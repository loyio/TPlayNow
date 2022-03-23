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
#include <iomanip>
#include <iostream>
#include <map>
#include <thread>

namespace asio = boost::asio;
using asio::ip::tcp;
using boost::system::error_code;
using namespace std::chrono_literals;
using std::this_thread::sleep_for;

/////// client //////////////////////////////////////////////////////////

struct RequestOp : public std::enable_shared_from_this<RequestOp> {
    using Callback = std::function<void( //
            unsigned /*request_id*/, std::string_view /*response*/, error_code)>;

    RequestOp(asio::any_io_executor ex, const std::string& raw_ip_address,
              uint16_t port_num, std::string request, unsigned id,
              Callback callback)
            : m_ep(asio::ip::address::from_string(raw_ip_address), port_num)
            , m_sock(ex, m_ep.protocol())
            , m_request(std::move(request))
            , m_id(id)
            , m_callback(callback) {}

    void Send() {
        m_sock.async_connect(
                m_ep, [this, self = shared_from_this()](error_code ec) {
                    if ((m_ec = ec) || m_was_cancelled)
                        return onComplete();

                    asio::async_write(m_sock, asio::buffer(m_request),
                                      [this, self = shared_from_this()](
                                              error_code ec, size_t /*bytes*/) {
                                          onRequestWritten(ec);
                                      });
                });
    }

    void Get() {
        m_sock.async_connect(
                m_ep, [this, self = shared_from_this()](error_code ec) {
                    if ((m_ec = ec) || m_was_cancelled)
                        return onComplete();
                    asio::async_read_until(m_sock, asio::dynamic_buffer(m_response), '\n', [this, self = shared_from_this()](error_code ec, size_t bytes){
                        onComplete();
                    });
                });
    }

    void Cancel() {
        m_was_cancelled = true;
        dispatch(m_sock.get_executor(), [self=shared_from_this()]{ self->doCancel(); });
    }

private:
    void doCancel() {
        m_sock.cancel();
    }

    void onRequestWritten(error_code ec) {
        if ((m_ec = ec) || m_was_cancelled)
            return onComplete();

        asio::async_read_until(
                m_sock, asio::dynamic_buffer(m_response), '\n',
                [this, self = shared_from_this()](error_code ec, size_t bytes) {
                    sendMsgComplete(ec, bytes);
                });
    }

    void sendMsgComplete(error_code ec, size_t /*bytes*/) {
        if ((m_ec = ec) || m_was_cancelled)
            return onComplete();

        if (!m_response.empty())
            m_response.resize(m_response.size() - 1); // drop '\n'

        onComplete();
    }

    void onComplete() {
        error_code ignored_ec;
        m_sock.shutdown(tcp::socket::shutdown_both, ignored_ec);

        if(!m_ec && m_was_cancelled){
            m_ec = asio::error::operation_aborted;
        }

        m_callback(m_id, m_response, m_ec);
    }

    tcp::endpoint m_ep; // Remote endpoint
    tcp::socket   m_sock;
    std::string   m_request;

    // streambuf where the response will be stored.
    std::string m_response; // Response represented as a string

    error_code m_ec;

    unsigned m_id;

    Callback m_callback;

    std::atomic_bool m_was_cancelled{false};
};

class AsyncTCPClient {
public:
    AsyncTCPClient(asio::any_io_executor ex, std::string raw_ip_address, uint16_t port_num) : m_executor(std::move(ex)), raw_ip_address(raw_ip_address), port_num(port_num) {}

    using Duration = std::chrono::steady_clock::duration;

    size_t GetMessage(RequestOp::Callback callback) {
        std::string request = "";
        auto const request_id = m_nextId++;
        auto session = std::make_shared<RequestOp>(make_strand(m_executor),
                                                   raw_ip_address, port_num, request, request_id, callback);

        std::unique_lock lock(m_active_sessions_guard);

        auto [_,ok] = m_pending_ops.emplace(request_id, session);
        assert(ok); // duplicate request_id?

        // optionally: garbage collect completed sessions
        std::erase_if(m_pending_ops,
                        [](auto& kv) { return kv.second.expired(); });
        while(true){
            session->Get();
        }
        return request_id;
    }

    size_t sendMessage(RequestOp::Callback callback){
        size_t request_id = 0;
        while(true){
            sleep_for(2s);
            std::string request = "";
            std::cout << "write your message: ";
            std::cin >> request;
            request_id++;
            auto session = std::make_shared<RequestOp>(make_strand(m_executor),
                                                    raw_ip_address, port_num, request, request_id, callback);

            {
                // active sessions list can be accessed from multiple thread, we
                // guard it with a mutex to avoid data coruption
                std::unique_lock lock(m_active_sessions_guard);

                auto [_,ok] = m_pending_ops.emplace(request_id, session);
                assert(ok); // duplicate request_id?

                // optionally: garbage collect completed sessions
                std::erase_if(m_pending_ops,
                            [](auto& kv) { return kv.second.expired(); });
            };
            session->Send();
        }
        return request_id;
    }

    // Cancels the request
    void cancelRequest(unsigned request_id) {
        std::unique_lock lock(m_active_sessions_guard);

        if (auto session = m_pending_ops[request_id].lock())
            session->Cancel();
    }

private:
    using PendingOp = std::weak_ptr<RequestOp>;

    asio::any_io_executor m_executor;
    std::string raw_ip_address;
    uint16_t port_num;
    std::mutex m_active_sessions_guard;
    size_t m_nextId = 1;
    std::map<int, PendingOp> m_pending_ops;
};

void handler(unsigned request_id, std::string_view response, error_code ec) {
    std::cout << "Request #" << request_id << " ";

    if (!ec.failed())
        std::cout << std::quoted(response) << std::endl;
    else if (ec == asio::error::operation_aborted)
        std::cout << "Cancelled" << std::endl;
    else
        std::cout << ec.message() << std::endl;
}

void client(uint16_t port){
    try {
        asio::thread_pool io;

        AsyncTCPClient client(io.get_executor(), "127.0.0.1", 3333);
        // auto GetMsgThread = client.GetMessage(handler);
        auto SendMsgThread = client.sendMessage(handler); 

        // cancel request 1
        //sleep_for(3s);
        //client.cancelRequest(GetMsgThread);

        //sleep_for(1200ms);
        // exit the application
        io.join();
    } catch (std::exception const& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}
 

int main(int argc, char**) {
   client(3333);
}

