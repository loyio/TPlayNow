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

void communicate(asio::ip::tcp::socket& sock){
	const char request_buf[] = {0x48, 0x65, 0x0, 0x6c, 0x6c, 0x6f};

	asio::write(sock, asio::buffer(request_buf));

	// shutting down the socket tot let the server know the size of the response message
	sock.shutdown(asio::socket_base::shutdown_send);

	// response message
	asio::streambuf response_buf;

	system::error_code ec;
	asio::read(sock, response_buf, ec);

	if(ec == asio::error::eof){
		// handle it
	}
	else{
		throw system::system_error(ec);
	}
}


int main(){
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	try{
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
		asio::io_service ios;

		asio::ip::tcp::socket sock(ios, ep.protocol());

		sock.connect(ep);

		communicate(sock);
	}
	catch(system::system_error &e){
		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();

		return e.code().value();
	}
	return 0;
}
