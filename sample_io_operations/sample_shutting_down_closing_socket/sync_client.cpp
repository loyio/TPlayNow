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
