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
