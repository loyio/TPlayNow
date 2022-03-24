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

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/steady_timer.hpp>
#include "../include/chat_message.hpp"

using boost::asio::steady_timer;
using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client{
public:
  chat_client(boost::asio::io_context& io_context) :
      stopped_(false),
      io_context_(io_context),
      socket_(io_context),
      deadline_(io_context),
      heartbeat_timer_(io_context){}

  void start(tcp::resolver::results_type endpoints){
      endpoints_ = endpoints;
      do_connect(endpoints.begin());

      deadline_.async_wait(boost::bind(&chat_client::check_deadline, this));

  }

  void write(const chat_message& msg){
    boost::asio::post(io_context_,[this, msg](){
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress){
            do_write();
        }
    });
  }

  void close(){
      boost::asio::post(io_context_, [this]() { socket_.close(); });
  }

private:

  void do_connect(const tcp::resolver::results_type::iterator endpoint_iter){
      if(endpoint_iter != endpoints_.end()){
          std::cout << "Trying " << endpoint_iter->endpoint() << "...\n";

          // Set a deadline for the connect operation.
          deadline_.expires_after(boost::asio::chrono::seconds(5));

          socket_.async_connect(endpoint_iter->endpoint(), boost::bind(&chat_client::handle_connect, this, boost::placeholders::_1, endpoint_iter));
      }
  }

  void handle_connect(const boost::system::error_code& ec, tcp::resolver::results_type::iterator endpoint_iter){
      if (stopped_)
        return;

      // The async_connect() function automatically opens the socket at the start
      // of the asynchronous operation. If the socket is closed at this time then
      // the timeout handler must have run first.
      if (!socket_.is_open())
      {
        std::cout << "Connect timed out\n";

        // Try the next available endpoint.
        do_connect(++endpoint_iter);
      }

      // Check if the connect operation failed before the deadline expired.
      else if (ec)
      {
        std::cout << "Connect error: " << ec.message() << "\n";

        throw std::invalid_argument("Connect Error");
        socket_.close();

        // Try the next available endpoint.
        do_connect(++endpoint_iter);
      }else{
        std::cout << "Connected to " << endpoint_iter->endpoint() << "\n";

        // Start the input actor.
        do_read_header();
      }
    }

  void async_stop(){
      socket_.cancel();
      socket_.close();
  }
  void do_read_header()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_read_body()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            std::cout.write(read_msg_.body(), read_msg_.body_length());
            std::cout << "\n";
            do_read_header();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_write(){
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            socket_.close();
          }
        });
  }

  void check_deadline(){
      if (stopped_)
        return;
      if (deadline_.expiry() <= steady_timer::clock_type::now()){
        socket_.close();
        deadline_.expires_at(steady_timer::time_point::max());
      }
      deadline_.async_wait(boost::bind(&chat_client::check_deadline, this));
  }

private:
  bool stopped_;
  boost::asio::io_context& io_context_;
  tcp::socket socket_;
  tcp::resolver::results_type endpoints_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
  steady_timer deadline_;
  steady_timer heartbeat_timer_;
};
