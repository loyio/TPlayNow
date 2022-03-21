#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {
    // ip address and port number
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    // get the error information
    boost::system::error_code ec;

    // parsing the ip address from string
    asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, ec);

    // ip address  is invalid, breaking
    if(ec.value() != 0){
        std::cout << "Failed to parse the IP address. Error code = " << ec.value() << ". Message: " << ec.message();
        return ec.value();
    }

    // create a endpoint
    asio::ip::tcp::endpoint ep(ip_address, port_num);


    return 0;
}

