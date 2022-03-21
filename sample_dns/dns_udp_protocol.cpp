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
