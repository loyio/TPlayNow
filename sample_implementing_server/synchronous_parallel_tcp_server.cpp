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
    Service(){}

    void StartHandlingClient(std::shared_ptr<asio::ip::tcp::socket> sock) {
        std::thread th(([this, sock](){
            HandleClient(sock);
        }));

        th.detach();
    }

private:
    void HandleClient(std::shared_ptr<asio::ip::tcp::socket> sock){
        try{
            std::string request;
            asio::read_until(*sock.get(), boost::asio::dynamic_buffer(request), '\n');
            std::cout << "request = " << request << std::endl;
            int i = 0;
            while(i != 100){
                i++;
                std::cout << "Processing i = " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            std::string response = "Response\n";
            asio::write(*sock.get(), asio::buffer(response));
        }
        catch (system::system_error& e){
            std::cout << "Error occurred ! Error code = " << e.code() << ". Message: " << e.what();
        }

        delete this;

    }
};

class Acceptor {
public:
    Acceptor(asio::io_service& ios, unsigned short port_num) : m_ios(ios), m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)){
        m_acceptor.listen();
    }

    void Accept() {
        std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(m_ios));

        m_acceptor.accept(*sock.get());

        (new Service)->StartHandlingClient(sock);
    }
private:
    asio::io_service& m_ios;
    asio::ip::tcp::acceptor m_acceptor;
};


class Server{
public:
    Server() : m_stop(false) {}

    void Start(unsigned short port_num) {
        m_thread.reset(new std::thread([this, port_num]() {
            Run(port_num);
        }));
    }

    void Stop(){
        m_stop.store(true);
        m_thread->join();
    }
private:
    void Run(unsigned short port_num) {
        Acceptor acc(m_ios, port_num);

        while (!m_stop.load()){
            acc.Accept();
        }
    }

    std::unique_ptr<std::thread> m_thread;
    std::atomic<bool> m_stop;
    asio::io_service m_ios;
};


int main(){
    unsigned short port_num = 3333;
    try{
        Server srv;
        srv.Start(port_num);

        std::this_thread::sleep_for(std::chrono::seconds(60));
        srv.Stop();
    }
    catch (system::system_error &e){
        std::cout << "Error Occurred! Error code = " << e.code() << ". Message: " << e.what();
    }
    return 0;
}