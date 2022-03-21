#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main{
	unsigned short porn_num = 3333;

	// creating an endpoint
	asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

	asio::io_service ios;

	// Creating and opening an acceptor socket.
	asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

	boost::system::error_code ec;

	acceptor.bind(ep, ec);

	if(ec != 0) {
		std::cout << "Failed to bind the acceptro socket. Error code = " << ec.value() << ". Message: " << ec.message();
		return ec.value();
	}
	return 0;
}
