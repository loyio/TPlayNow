#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main{
	std::string hots = "samplehost.book";
	std::string port_num = "3333";

	asio::io_service ios;

	// query
	asio::ip::tcp::resolver::query resolver_query(host, port_num, asio::ip::tcp::resolver::query::numeric_service);

	// Creating a resolver
	asio::ip::tcp::resolver resolver(ios);

	try{
		asio::ip::tcp::reolver::iterator it = resolver.resolve(resolver_query);

		asio::ip::tcp::socket sock(ios);

		asio::connect(sock, it);
	}
	catch(system::system_error &e){
		std::cout < "Error occured! Error code = " << e.code() << ". Message: " << e.what();
		return e.code().value();
	}
	return 0;
}
