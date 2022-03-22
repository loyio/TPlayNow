/*
MIT License

Copyright (c) 2022 Loyio Hex

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 */

#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

class SyncTCPClient{
public:
	SyncTCPClient(const std::string& raw_ip_address, unsigned short port_num): m_ep(asio::ip::address::from_string(raw_ip_address),port_num), m_sock(m_ios){
		m_sock.open(m_ep.protocol()); }

	void connect(){
		m_sock.connect(m_ep);
	}

	void close(){
		m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		m_sock.close();
	}

	std::string emulateLongComputationOp(unsigned int duration_sec){
		std::string request  = "EMULATE_LONG_COMP_OP " + std::to_string(duration_sec) + "\n";

		sendRequest(request);

		return receiveResponse();
	}
private:
	void sendRequest(const std::string& request){
		asio::write(m_sock, asio::buffer(request));
	}

	std::string receiveResponse(){
		asio::streambuf buf;
		asio::read_until(m_sock, buf, '\n');
		std::istream input(&buf);

		std::string response;
		std::getline(input, response);

		return response;
	}
private:
	asio::io_service m_ios;
	
	asio::ip::tcp::endpoint m_ep;
	asio::ip::tcp::socket m_sock;
};


int main(){
	const std::string raw_ip_address = "127.0.0.1";
	const unsigned short port_num = 3333;

	try{
		SyncTCPClient client(raw_ip_address, port_num);

		// Sync connect
		client.connect();

		std::cout << "Sending request to the server..." << std::endl;

		std::string response = client.emulateLongComputationOp(10);

		std::cout << "Response received: " << response << std::endl;

		// close the connection and free resource
		client.close();
	}
	catch(system::system_error &e){
		std::cout << "Error occured! Error code: " << e.code() << ". Message: " << e.what();
		return e.code().value();
	}

	return 0;
}
