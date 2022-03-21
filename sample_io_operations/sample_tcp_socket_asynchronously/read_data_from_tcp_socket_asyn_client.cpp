#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

// a socket object, a pointer to the buffer, a variable contains the size of the buffer, and a total_bytes_read variable contains the number of bytes already read
struct Session{
	std::shared_ptr<asio::ip::tcp::socket> sock;
	std::unique_ptr<char[]>  buf;
	std::size_t total_bytes_read;
	unsigned int buf_size;
};


// called when the asynchronous operations is completed
void callback(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<Session> s){
	if(ec.value() != 0){
		std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
		return;
	}
	s->total_bytes_read += bytes_transferred;

	if(s->total_bytes_read == s->buf_size){
		return;
	}

	s->sock->async_read_some(asio::buffer(s->buf.get() + s->total_bytes_read, s->buf_size - s->total_bytes_read), std::bind(callback, std::placeholders::_1, std::placeholders::_2, s));
}

void readFromSocket(std::shared_ptr<asio::ip::tcp::socket> sock){
	std::shared_ptr<Session> s(new Session);

	const unsigned int MESSAGE_SIZE = 7;
	
	s->buf.reset(new char [MESSAGE_SIZE]);
	s->total_bytes_read = 0;
	s->sock = sock;
	s->buf_size = MESSAGE_SIZE;

	s->sock->async_read_some(asio::buffer(s->buf.get(), s->buf_size), std::bind(callback, std::placeholders::_1, std:: placeholders::_2, s));
}

int main(){
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	try{
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
		asio::io_service ios;

		std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(ios, ep.protocol()));

		sock->connect(ep);

		readFromSocket(sock);

		ios.run();
	}catch(system::system_error &e){
		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
		return e.code().value();
	}
}
