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

int main(){
	// queue containing the pending connection requests
	const int BACKLOG_SIZE = 30;

	unsigned short port_num = 3333;

	asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);
	asio::io_service ios;

	try{
		// opening acceptor socket
		asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

		acceptor.bind(ep);
		acceptor.listen(BACKLOG_SIZE);

		asio::ip::tcp::socket sock(ios);

		acceptor.accept(sock);

	}
	catch(system::system_error &e){
		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}
