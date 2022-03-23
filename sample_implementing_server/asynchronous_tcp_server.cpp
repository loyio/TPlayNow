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

#include <boost/asio.hpp>

#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

using namespace boost;

class Service {
public:
    Service(std::shared_ptr<asio::ip::tcp::socket> sock) : m_sock(sock) {}

    void StartHandling() {
        asio::async_read_until(*m_sock.get(), asio::dynamic_buffer(m_request), '\n', [this](const boost::system::error_code& ec, std::size_t bytes_transferred){
            onRequestReceived(ec, bytes_transferred);
        });
        std::cout << "m_request : " << m_request << std::endl;
    }

private:
    void onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transfered){
        if (ec.value() != 0){
            std::cout << "Error occurred! Error code = " << ec.value() << ".Message: " << ec.message();
            onFinish();
            return;
        }
        m_response = ProcessingRequest(m_request);
        // Process the request
        asio::async_write(*m_sock.get(), asio::buffer(m_response), [this](const boost::system::error_code& ec, std::size_t bytes_transferred){
            onResponseSent(ec, bytes_transferred);
        });
    }

    void onResponseSent(const boost::system::error_code& ec, std::size_t bytes_transferred){
        if(ec.value() != 0){
            std::cout << "Error occurred! Error code = " << ec.value() << ". Message: " << ec.message();
        }

        onFinish();
    }

    // cleanup
    void onFinish(){
        delete this;
    }

    std::string ProcessingRequest(std::string request){
        // parse the request, process it and prepare the request

        // Emulating CPU-consuming operations
        int i = 0;
        while (i != 1000){
            i++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::string response = "Response\n";
        return response;
    }

    std::shared_ptr<asio::ip::tcp::socket> m_sock;
    std::string m_response;
    std::string m_request;
};


class Acceptor {
public:
    Acceptor(asio::io_service& ios, unsigned short port_num) : m_ios(ios), m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)), m_isStopped(
            false) {}
    // Start accepting incoming connection request.
    void Start(){
        m_acceptor.listen();
        InitAccept();
    }

    void Stop() {
        m_isStopped.store(true);
    }

private:
    void InitAccept() {
        std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(m_ios));

        m_acceptor.async_accept(*sock.get(), [this, sock](const boost::system::error_code& error){
            onAccept(error, sock);
        });
    }

    void onAccept(const boost::system::error_code& ec, std::shared_ptr<asio::ip::tcp::socket> sock){
        if(ec.value() == 0){
            (new Service(sock))->StartHandling();
        }else{
            std::cout << "Error occurred! Error code = " << ec.value() << ". Message: " << ec.message();
        }

        // Init next accept operation if acceptor has not been stopped yet
        if(!m_isStopped.load()){
            InitAccept();
        }else{
            // free resources
            m_acceptor.close();
        }
    }

private:
    asio::io_service& m_ios;
    asio::ip::tcp::acceptor m_acceptor;
    std::atomic<bool> m_isStopped;
};


class Server{
public:
    Server() {
       m_work.reset(new asio::io_service::work(m_ios));
    }

    // Start the server
    void Start(unsigned short port_num, unsigned int thread_pool_size){
        assert(thread_pool_size > 0);

        // Create and start Acceptor
        acc.reset(new Acceptor(m_ios, port_num));
        acc->Start();

        // Create specified number of thread and add them to the pool
        for(unsigned int i = 0; i < thread_pool_size; i++){
            std::cout << "Thread " << i << " Running !";
            std::unique_ptr<std::thread> th(new std::thread([this](){
                m_ios.run();
            }));
            m_thread_pool.push_back(std::move(th));
        }
    }

    // Stop the Server
    void Stop(){
        acc->Stop();
        m_ios.stop();

        for(auto& th : m_thread_pool){
            th->join();
        }
    }

private:
    asio::io_service m_ios;
    std::unique_ptr<asio::io_service::work> m_work;
    std::unique_ptr<Acceptor> acc;
    std::vector<std::unique_ptr<std::thread>> m_thread_pool;
};

const unsigned int DEFAULT_THREAD_POOL_SIZE = 2;

int main(){
    unsigned short port_num = 3333;

    try{
        Server srv;

        unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;

        if (thread_pool_size == 0){
            thread_pool_size = DEFAULT_THREAD_POOL_SIZE;
        }

        srv.Start(port_num, thread_pool_size);

        std::this_thread::sleep_for(std::chrono::seconds(60));

        srv.Stop();
    }
    catch(system::system_error &e){
        std::cout << "Error occurred! Error code = " << e.code() << ". Message: " << e.what();
    }

    return 0;
}