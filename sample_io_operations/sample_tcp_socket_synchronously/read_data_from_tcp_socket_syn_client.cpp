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

std::string readFromSocket(asio::ip::tcp::socket& sock){
	const unsigned char MESSAGE_SIZE = 7;
	char buf[MESSAGE_SIZE];
	std::size_t total_bytes_read = 0;

	while(total_bytes_read != MESSAGE_SIZE){
		total_bytes_read += sock.read_some(asio::buffer(buf + total_bytes_read, MESSAGE_SIZE - total_bytes_read));
	}

	return std::string(buf, total_bytes_read);
}

std::string readFromSocketDelim(asio::io::tcp::socket& sock){
	asio::streambuf buf;

	asio::read_until(sock, buf, '\n');

	std::string message;

	std::istream input_stream(&buf);
	std::getline(input_stream, message);
	return message;
}

int main(){
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num  = 3333;

	try{
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);

		asio::io_service ios;

		asio::ip::tcp::socket sock(ios, ep.protocol());

		sock.connect(ep);

		readFromSocket(sock);
	}catch(system::system_error &e){
		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}
