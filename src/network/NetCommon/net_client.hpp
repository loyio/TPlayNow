#/*******************************************************************
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
#pragma once

#include "net_common.hpp"
#include "net_connection.hpp"


namespace tplayn
{
    namespace net
    {
        template <typename T>
        class client_interface{
        public:
            client_interface(){}

            virtual ~client_interface() { Disconnect(); }


        public:

            bool Connect(const std::string& host, const uint16_t port){
                try{
                    // resolve the ipaddress into tangiable physical address
                    boost::asio::ip::tcp::resolver resolver(m_ioc);
                    boost::asio::ip::tcp::resolver::results_type m_endpoints = resolver.resolve(host, std::to_string(port));

					// Create connection
					m_connection = std::make_unique<connection<T>>(connection<T>::owner::client, m_ioc, boost::asio::ip::tcp::socket(m_ioc), m_qMessagesIn);
					
					// Tell the connection object to connect to server
					m_connection->ConnectToServer(m_endpoints);

                    // Start Context Thread
                    thrContext = std::thread([this]() { m_ioc.run(); });
                }
                catch(std::exception& e){
                    std::cerr << "Client Exception: " << e.what() << std::endl;
                    return false;
                }


                return true;
            }


            void Disconnect(){
                if(IsConnected()){
                    m_connection->Disconnect();

                }

                m_ioc.stop();
                if(thrContext.joinable()){
                    thrContext.join();
                }


                // Destroy the connection object
                m_connection.release();
            }


            bool IsConnected(){
                if(m_connection)
                    return m_connection->IsConnected();
                else 
                    return false;
            }
        public:
            // Send message to server
            void Send(const message<T>& msg){
                if(IsConnected()){
                    m_connection->Send(msg);
                }
            }

            // get queue of messsage from server
            tsqueue<owned_message<T>>& Incoming(){
                return m_qMessagesIn;
            }

        protected:
            // context handle data transfer
            boost::asio::io_context m_ioc;
            
            // needs a thread to execute its work commands
            std::thread thrContext;

            // the client has a single instance of a "connection" object, which handles data transfer
            std::unique_ptr<connection<T>> m_connection;

        private:
            // the thread safe queue of incomming message from server
            tsqueue<owned_message<T>> m_qMessagesIn;

        };
    }
}
