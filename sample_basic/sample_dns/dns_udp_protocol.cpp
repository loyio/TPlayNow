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

using namespace std;

int main(){
	// already get the DNS name and port
	std::string host = "samplehost.book";
	std::string port_num = "3333";

	asio::io_service ios;

	// query
	asio::ip::udp::resolver::query resolver_query(host, port_num, asio::ip::udp::resolver::query::numeric_service);

	// creating a resolver
	asio::ip::udp::reolver resolver(ios);

	// store information about error
	boost::system::error_code ec;

	asio::ip::udp::resolver::iterator it = resolver.resolve(resolver_query, ec);

	// handling errors
	if(ec != 0){
		std::cout << "Failed to resolve a DNS name. Error code =  " << ec.value() << ". Message = " << ec.message();
		return ec.value();
	}

	asio::ip::udp::resolve::iterator it_end;

	for(; it != it_end; ++it){
		asio::ip::udp::endpoint ep = it->endpoint();
	}

	return 0;

}
