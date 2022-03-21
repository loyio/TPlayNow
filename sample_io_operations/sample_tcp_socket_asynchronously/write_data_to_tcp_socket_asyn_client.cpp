#include <boost/asio.hpp>
#include <iostream>
#include <functional>

using namespace boost;

// a pointer to the socket object, a buffer that contains data to be written, and a counter variable that contains the number of bytes already written
struct Session{
	std::shared_ptr<asio::ip::tcp::socket> sock;
	std::string buf;
	std::size_t total_bytes_written;
};


// called when the asynchronous operations
void callback(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<Session> s){
	if(ec.value() != 0){
		std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
		return;
	}
	s->total_bytes_written += bytes_transferred;

	if(s->total_bytes_written == s->buf.length()){
		return;
	}

	s->sock->async_write_some(asio::buffer(s->buf.c_str() + s->total_bytes_written, s->buf.length() - s->total_bytes_written), std::bind(callback, std::placeholders::_1, std::placeholders::_2, s));
}

void writeToSocket(std::shared_ptr<asio::ip::tcp::socket> sock){
	std::shared_ptr<Session> s(new Session);

	s->buf = std::string("Hello");
	s->total_bytes_written = 0;
	s->sock = sock;

	s->sock->async_write_some(asio::buffer(s->buf), std::bind(callback, std::placeholders::_1, std:: placeholders::_2, s));
}

int main(){
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	try{
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
		asio::io_service ios;

		std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(ios, ep.protocol()));

		sock->connect(ep);

		writeToSocket(sock);

		ios.run();
	}catch(system::system_error &e){
		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
		return e.code().value();
	}
}
