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
#include <iostream>

using namespace boost;

int main() {
    // ip address and port number
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    // get the error information
    boost::system::error_code ec;

    // parsing the ip address from string
    asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, ec);

    // ip address  is invalid, breaking
    if(ec.value() != 0){
        std::cout << "Failed to parse the IP address. Error code = " << ec.value() << ". Message: " << ec.message();
        return ec.value();
    }

    // create a endpoint
    asio::ip::tcp::endpoint ep(ip_address, port_num);


    return 0;
}

