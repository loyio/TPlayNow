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

void processRequest(asio::ip::tcp::socket& sock){
	asio::streambuf request_buf;

	system::error_code ec;

	asio::read(sock, request_buf, ec);

	if(ec != asio::error::eof)
		throw system::system_error(ec);

	const char response_buf[] = { 0x48, 0x69, 0x21 };

	// sending the request data
	asio::write(sock, asio::buffer(response_buf));

	sock.shutdown(asio::socket_base::shutdown_send);

}


int main(){
	unsigned short port_num = 3333;

	try{
		asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

		asio::io_service ios;

		asio::ip::tcp::acceptor acceptor(ios, ep);

		asio::ip::tcp::socket sock(ios);

		acceptor.accept(sock);

		processRequest(sock);

	}
	catch(system::system_error &e){
		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();

		return e.code().value();
	}

	return 0;
}
