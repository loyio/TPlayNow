#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main(){
	std::string buf;
	buf = "Hello Loyio";

	// Creating buffer representation that satisfies const buffer sequeunce concept
	asio::const_buffers_1 output_buf = asio::buffer(buf);

	// output_buf is the representation of the buffer 'buf' can be used in asio output operations
	return 0;
}
