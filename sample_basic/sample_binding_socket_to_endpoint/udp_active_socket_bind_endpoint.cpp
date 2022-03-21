#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main(){
	unsigned short port_num = 3333;

	asio::ip::udp::endpoint ep(asio::ip::address_v4::any(), port_num);

	asio::io_service ios;
	
	// creating and open a socket
	asio::ip::udp::socket sock(ios, ep.protocol());
	boost::system::error_code ec;

	sock.bind(ep, ec);

	if(ec != 0) {
		std::cout << "Failed to bidn the socket. Error code = " << ec.value() << ". Message: " << ec.message();
		return ec.value();
	}

	return 0;
}
