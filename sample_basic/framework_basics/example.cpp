/*******************************************************************
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
********************************************************************/
#include<iostream>

#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using namespace boost;

std::vector<char> vBuffer(1*1024);

void GrabSomeData(asio::ip::tcp::socket& socket){
  socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
      [&](std::error_code ec, std::size_t length){
        if(!ec){
          std::cout << "\n\nRead " << length << " bytes\n\n";

          for(int i = 0; i < length; ++i){
            std::cout << vBuffer[i];
          }

          GrabSomeData(socket);
        }
      });
}

int main(){
  boost::system::error_code ec;
  
  asio::io_context ioc;


  // Give some fake tasks to asio so the context doesn't finish
  asio::io_context::work idleWork(ioc);

  // start the context
  std::thread thrContext = std::thread([&]() { ioc.run(); });

  // example.com
  // asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);
  asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);
    

  asio::ip::tcp::socket sock(ioc);

  sock.connect(endpoint, ec);
  
  if(!ec){
    std::cout <<  "Connected! " << std::endl;
  }else{
    std::cout << "Failed to connect to address : \n" << ec.message() << std::endl;
  }

  if(sock.is_open()){

    GrabSomeData(sock);

    std::string sRequest = 
      "GET /index.html HTTP/1.1\r\n"
      "Host: example.com\r\n"
      "Connection: close\r\n\r\n";
    sock.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20000ms);
    
    ioc.stop();
    if(thrContext.joinable()) thrContext.join();
    /*
    sock.wait(sock.wait_read);

    size_t bytes = sock.available();
    std::cout << "Bytes Available: " << bytes << std::endl;


    if(bytes > 0){
      std::vector<char> vBuffer(bytes);
      sock.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);

      for(auto c : vBuffer){
        std::cout << c;
      }
    }
    */

  }

  return 0;
}
