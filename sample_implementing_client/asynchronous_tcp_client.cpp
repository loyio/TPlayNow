/*
MIT License

Copyright (c) 2022 Loyio Hex

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 */


#include <boost/predef.h> // Tools to identify the os

#ifdef BOOST_OS_WINDOWS
#define _WIN32_WINNT 0x0501

#if _WIN32_WINNT <= 0x0502
	#define BOOST_ASIO_DISABLE_TOCP
	#define BOOST_ASIO_ENABLE_CANCELIO
#endif
#endif

#include <boost/asio.hpp>
#include <mutex>
#include <thread>
#include <memory>
#include <iostream>
#include <map>

using namespace boost;


typedef void(*Callback) (unsigned int request_id, const std::string& response, const system::error_code& ec);

struct Session{
	Session(asio::io_service& ios, const std::string& raw_ip_address, unsigned short port_num, const std::string& request, unsigned int id, Callback callback) : m_sock(ios), m_ep(asio::ip::address::from_string(raw_ip_address),port_num), m_request(request), m_id(id), m_callback(callback), m_was_cancelled(false) {} 

	asio::ip::tcp::socket m_sock;
	asio::ip::tcp::endpoint m_ep; // Remote endpoint
	std::string m_request;

	// streambuf where the response will be stored.
	asio::streambuf m_response_buf;
	std::string m_response; // Response represented as a string

	system::error_code m_ec;

	unsigned int m_id;

	Callback m_callback;

	bool m_was_cancelled;
	std::mutex m_cancel_guard;
};

class AsyncTCPClient : public boost::asio::noncopyable {
public: 
	AsyncTCPClient(){
		m_work.reset(new boost::asio::io_service::work(m_ios));

		m_thread.reset(new std::thread([this](){
			m_ios.run();
		}));
	}


	void emulateLongComputationOp( unsigned int duration_sec, const std::string& raw_ip_address, unsigned short port_num, Callback callback, unsigned int request_id){
		std::string request = "EMULATE_LONG_CALC_OP " + std::to_string(duration_sec) + "\n";
        std::cout << "Request: " << request << std::endl;

		std::shared_ptr<Session> session = std::shared_ptr<Session> (new Session(m_ios, raw_ip_address, port_num, request, request_id, callback));

		session->m_sock.open(session->m_ep.protocol());

		// active sessions list can be accessed from multiple thread, we guard it with a mutex to avoid data coruption
		std::unique_lock<std::mutex> lock(m_active_sessions_guard);
		m_active_sessions[request_id] = session;
		lock.unlock();

		session->m_sock.async_connect(session->m_ep, [this, session](const system::error_code& ec) {
            if (ec.value() != 0) {
                session->m_ec = ec;
                onRequestComplete(session);
                return;
            }

            std::unique_lock<std::mutex> cancel_lock(session->m_cancel_guard);

            if (session->m_was_cancelled) {
                onRequestComplete(session);
                return;
            }

            asio::async_write(session->m_sock, asio::buffer(session->m_request), [this, session](const boost::system::error_code &ec, std::size_t bytes_transferred) {
                if (ec.value() != 0) {
                    session->m_ec = ec;
                    onRequestComplete(session);
                    return;
                }
                std::unique_lock<std::mutex> cancel_lock(session->m_cancel_guard);

                if (session->m_was_cancelled) {
                    onRequestComplete(session);
                    return;
                }
                
                asio::async_read_until(session->m_sock, session->m_response_buf, '\n',
                                       [this, session](const boost::system::error_code &ec,
                                                       std::size_t bytes_transferred) {
                                           if (ec.value() != 0) {
                                               session->m_ec = ec;
                                           } else {
                                               std::istream strm(&session->m_response_buf);
                                               std::getline(strm, session->m_response);
                                           }

                                           onRequestComplete(session);
                                       });
            });
        });
    };

    // Cancels the request
    void cancelRequest(unsigned int request_id){
        std::unique_lock<std::mutex> lock(m_active_sessions_guard);

        auto it = m_active_sessions.find(request_id);
        if(it != m_active_sessions.end()){
            std::unique_lock<std::mutex> cancel_lock(it->second->m_cancel_guard);

            it->second->m_was_cancelled = true;
            it->second->m_sock.cancel();
        }
    }


    void close(){
        // Destroy work object
        m_work.reset(NULL);
        // wait for the I/O thread tot exit
        m_thread->join();
    }

private:
    void onRequestComplete(std::shared_ptr<Session> session){
        // shutting down the connection, we don't care about the error code if function failed
        boost::system::error_code ignored_ec;

        session->m_sock.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);

        // remove session from the map of active sessions
        std::unique_lock<std::mutex> lock(m_active_sessions_guard);

        auto it = m_active_sessions.find(session->m_id);
        if(it != m_active_sessions.end()){
            m_active_sessions.erase(it);
        }

        lock.unlock();

        boost::system::error_code ec;

        if(session->m_ec.value() == 0 && session->m_was_cancelled){
            ec = asio::error::operation_aborted;
        }else{
            ec = session->m_ec;
        }

        session->m_callback(session->m_id, session->m_response, ec);
    };
private:
	asio::io_service m_ios;
	std::map<int, std::shared_ptr<Session>> m_active_sessions;
	std::mutex m_active_sessions_guard;
	std::unique_ptr<boost::asio::io_service::work> m_work;
	std::unique_ptr<std::thread> m_thread;
};


void handler(unsigned int request_id, const std::string& response, const system::error_code& ec){
    if(ec.value() == 0){
        std::cout << "Request #" << request_id << " has completed. Reponse: "<< response << std::endl;
    }else if(ec == asio::error::operation_aborted){
        std::cout << "Request #" << request_id << " has been cancelled by the user. "  << std::endl;
    }else{
        std::cout << "Request #" << request_id << " failed! Error code = " << ec.value() << ". Error Message = " << ec.message() << std::endl;
    }
    return;
}


int main(){
    try{
        AsyncTCPClient client;

        // emulate the user's behavior
        client.emulateLongComputationOp(10, "127.0.0.1", 3333, handler, 1);

        std::this_thread::sleep_for(std::chrono::seconds(60));

        // another request with id 2
        client.emulateLongComputationOp(11, "127.0.0.1", 3334, handler, 2);

        // cancel request 1
        client.cancelRequest(1);

        std::this_thread::sleep_for(std::chrono::seconds(6));

        // another request with id 3
        client.emulateLongComputationOp(12, "127.0.0.1", 3335, handler, 3);

        std::this_thread::sleep_for(std::chrono::seconds(15));

        // exit the application
        client.close();
    }
    catch(system::system_error &e){
        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}