#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main(){
	asio::streambuf buf;

	std::ostream output(&buf);

	output << "Message1\nMessage2";

	// read all data from a streambuf until '\n' delimiter
	std::istream input(&buf);

	std::string message1;

	std::getline(input, message1);

	return 0;
}
