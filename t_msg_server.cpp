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
#include <utility>
#include <boost/spirit/home/x3.hpp> // for request parsing
#include <iomanip>
#include <iostream>
#include <map>
#include <thread>

namespace asio = boost::asio;
using asio::ip::tcp;
using boost::system::error_code;
using namespace std::chrono_literals;
using std::this_thread::sleep_for;

namespace x3 = boost::spirit::x3;

namespace my_parser {
    x3::rule<class identifier_rule_, std::string> const identifier_rule = "identifier_rule";
    auto const identifier_rule_def = x3::lexeme[(x3::alpha | x3::char_('_')) >> *(x3::alnum | x3::char_('_'))];
    BOOST_SPIRIT_DEFINE(identifier_rule)
}


// Processing Service
struct Service : std::enable_shared_from_this<Service> {
    explicit Service(tcp::socket sock) : m_sock(std::move(sock)) {}

    void StartHandling() {
        async_read_until(m_sock, asio::dynamic_buffer(m_request), '\n',[this, self = shared_from_this()](error_code ec, size_t bytes) {
                    onRequestReceived(ec, bytes);
        });
        std::cout << "messsage get: " << m_request << std::endl;
    }

private:
    void onRequestReceived(error_code ec, size_t /*bytes*/) {
        if(ec.value() == 0){
            std::cout << "onRequestReceived: Success!" << std::endl;
        }else{
            std::cout << "onRequestReceived: " <<  ec.message() << std::endl;
        }
        if (ec)
            return;

        // Process the request
        m_response = ProcessingRequest(m_request);

        async_write(m_sock, asio::buffer(m_response),[this, self = shared_from_this()](error_code ec, size_t bytes) {
                    onResponseSent(ec, bytes);
        });
    }

    void onResponseSent(error_code ec, size_t /*bytes*/) {
        if(ec.value() == 0){
            std::cout << "onResponseSent: Success!" << std::endl;
        }else{
            std::cout << "onResponseSent: " <<  ec.message() << std::endl;
        }
    }

    std::string static ProcessingRequest(std::string request) {
        std::cout << "request: " << request << std::endl;

        // parse the request, process it and prepare the response

        std::string messages;
        if (parse(request.begin(), request.end(),
                  "T_MSG_NOW_" >> my_parser::identifier_rule >> "s" >> x3::eol >> x3::eoi,
                  messages)){
            // Emulating time-consuming operation
            return "Your message is :" + messages + "s\n";
        }

        return "Unknown request\n";
    }

    tcp::socket m_sock;
    std::string m_request, m_response;
};


struct Server {
    Server(asio::any_io_executor ex, uint16_t port_num)
            : m_acceptor{ex, {{}, port_num}} {
        m_acceptor.listen();
        accept_loop();
    }

    void Stop() { m_acceptor.cancel(); }

private:
    void accept_loop() {
        m_acceptor.async_accept([this](error_code ec, tcp::socket sock) {
            std::cout << "OnAccept: " << ec.value() << std::endl;
            if (!ec) {
                std::make_shared<Service>(std::move(sock))->StartHandling();
                accept_loop();
            }
            //m_acceptor.close();
        });
    }

    tcp::acceptor m_acceptor;
};

void server(uint16_t port) try {
    asio::thread_pool io;
    Server srv{io.get_executor(), port};
    io.join();
} catch (std::exception const& e) {
    std::cout << "Exception: " << e.what() << std::endl;
}

int main(){

    server(3333);
}
