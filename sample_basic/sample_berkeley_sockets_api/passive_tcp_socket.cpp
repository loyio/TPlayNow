#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main(){
	// instance of "io_service"
	asio::io_service ios;

	// object of 'tcp'
	asio::ip::tcp protocol = asio::ip::tcp::v6();

	// Instantiating an acceptor socket object
	asio::ip::tcp::acceptor acceptor(ios);

	// error info
	boost::system::error_code ec;

	// opening the acceptor socket
	acceptor.open(protocol, ec);

	if(ec.value() != 0){
		std::cout << "Failed to open the acceptor socket ! Error code = " << ec.value() << ". Message: " << ec.message();
		return ec.value();
	}

	return 0;
}
