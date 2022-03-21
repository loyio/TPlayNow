#include<boost/asio.hpp>
#include<iostream>

using namespace boost;

int main(){
	// "io_service" required by socket constructor
	asio::io_service ios;

	// object of udp
	asio::ip::udp protocol = asio::ip::udp::v6();

	// Instantiating an active TCP socket
	asio::ip::udp::socket sock(ios);

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
