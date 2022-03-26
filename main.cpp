#include<iostream>

#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using namespace boost;

int main(){
  boost::system::error_code ec;
  
  asio::io_context ioc;

  asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);

  asio::ip::tcp::socket socket(ioc);
  
  if(!ec){
    std::cout <<  "Connected! " << std::endl;
  }else{
    std::cout << "Failed to connect to address : \n" << ec.message() << std::endl;
  }

  if(socket.is_open()){
    std::string sRequest = 
      "GET /index.html HTTP/1.1\r\n"
      "Host: example.com\r\n"
      "Connection: close\r\n\r\n";
    socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

    size_t bytes = socket.available();
    std::cout << "Bytes Available: " << bytes << std::endl;


    if(bytes > 0){
      std::vector<char> vBuffer(bytes);
      socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);

      for(auto c : vBuffer){
        std::cout << c;
      }
    }
  }

  return 0;
}
