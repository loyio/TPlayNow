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

#include<boost/asio.hpp>
#include<iostream>

using namespace boost;

int main(){
	// "io_service" required by socket constructor
	asio::io_service ios;

	// object of tcp
	asio::ip::tcp protocol = asio::ip::tcp::v4();

	// Instantiating an active TCP socket
	asio::ip::tcp::socket sock(ios);

	// store error info
	boost::system::error_code ec;

	// open socket
	sock.open(protocol, ec);

	if(ec.value() != 0){
		std::cout << "Failed to open the socket! Error code  = " << ec.value() << ". Message: " << ec.message();
		return ec.value();
	}

	return 0;
}
