#pragma once

#include "net_common.hpp"
#include "net_tsqueue.hpp"
#include "net_message.hpp"

namespace tplayn
{ namespace net
    {
        template<typename T>
        class connection : public std::enable_shared_from_this<connection<T>>{
        public:

            // connection is "owned" by server or client, behaviour will different
            enum class owner{
                server,
                client
            };

        public:

            connection(owner parent, boost::asio::io_context& ioc, boost::asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
                : m_ioc(ioc), m_socket(std::move(socket)), m_qMessagesIn(qIn){
                    m_nOwnerType = parent;
            }

            virtual ~connection() {}


            uint32_t GetID() const{
                return id;
            }
            
        public:

            void ConnectToClient(uint32_t uid = 0){
                if(m_nOwnerType == owner::server){
                    if(m_socket.is_open()){
						std::cout << "Begin to read Header" << std::endl;
                        id = uid;
                        ReadHeader();
                    }
                }
            }

            void ConnectToServer(const boost::asio::ip::tcp::resolver::results_type& endpoints){
                // get Client
                if(m_nOwnerType == owner::client){
                    boost::asio::async_connect(m_socket, endpoints, [this](std::error_code ec, boost::asio::ip::tcp::endpoint endpoint){
                                if(!ec){
                                    ReadHeader();
                                }

                            });
                }

            }
            void Disconnect(){
                if(IsConnected()){
                    boost::asio::post(m_ioc, [this]() { m_socket.close(); });
                }
            }

            bool IsConnected() const{
                return m_socket.is_open();
            }


            // wait for incoming message
            void StartListening(){
            }

        public:
            void Send(const message<T>& msg){
                boost::asio::post(m_ioc, 
					[this, msg](){
                            bool bWritingMessage = !m_qMessagesOut.empty();
							std::cout << "Send Message : " << msg << std::endl;
                            m_qMessagesOut.push_back(msg);
                            if(!bWritingMessage){
                                WriteHeader();
                            }
                        });
            }


        private:
            // Async write a message header
            void WriteHeader(){
                boost::asio::async_write(m_socket, boost::asio::buffer(&m_qMessagesOut.front().header, sizeof(message_header<T>)),
                        [this](std::error_code ec, std::size_t length){
                            if(!ec){
                                if(m_qMessagesOut.front().body.size() > 0){
									std::cout << "Begin to WriteBody : (size) " << m_qMessagesOut.front().body.size() << std::endl;
                                    WriteBody();
                                }else{
                                    m_qMessagesOut.pop_front();

                                    if(!m_qMessagesOut.empty()){
                                        WriteHeader();
                                    }
                                }
                                
                            }else{
                                std::cout << "[" << id << "] Write Header Fail. \n";
                                m_socket.close();
                            }
                        });
            }

            // Async write a message body
            void WriteBody(){
                boost::asio::async_write(m_socket, boost::asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
                        [this](std::error_code ec, std::size_t length){
                            if(!ec){
								std::cout << "Write body Success ! " << m_qMessagesOut.front() << std::endl;
                                m_qMessagesOut.pop_front();

                                if(!m_qMessagesOut.empty()){
                                    WriteHeader();
                                }
                            }else{
                                std::cout << "[" << id << "] Write Body Fail. \n";
                                m_socket.close();
                            }
                        });
            }


            // Async Read a message header
            void ReadHeader(){
                boost::asio::async_read(m_socket, boost::asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header<T>)), 
                        [this](std::error_code ec, std::size_t length){
                            if(!ec){
								std::cout << "Read header success ! : " << m_msgTemporaryIn << std::endl;
                                if(m_msgTemporaryIn.header.size > 0){
                                    m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size);
                                    ReadBody();
                                }else{
                                    AddToIncomingMessageQueue();
                                }
                            }else{
                                std::cout << "[" << id << "] Read Header Fail.\n";
                                m_socket.close();
                            }
                        });
            }


            // Async ready to read a messag body
            void ReadBody(){
				std::cout << "Begin to read body !!" << m_msgTemporaryIn <<std::endl;
                boost::asio::async_read(m_socket, boost::asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),
                        [this](std::error_code ec, std::size_t length){
                            if(!ec){
								std::cout << "Read Body Success ! " << m_msgTemporaryIn << std::endl;
                                AddToIncomingMessageQueue();
                            }else{
                                std::cout << "[" << id << "] Read Body Fail. \n";
                                m_socket.close();
                            }
                        });
            }


            // Once a full message is received, add it to the incoming queue
            void AddToIncomingMessageQueue(){
                if(m_nOwnerType == owner::server){
                    m_qMessagesIn.push_back({ this->shared_from_this(), m_msgTemporaryIn });
                }else{
                    std::cout << "AddToIncomingMessageQueue : " << m_msgTemporaryIn << std::endl;
                    m_qMessagesIn.push_back( {nullptr, m_msgTemporaryIn });
                }

                ReadHeader();
            }



        protected:
            // unique socket to remote
            boost::asio::ip::tcp::socket m_socket;

            // this context is shared with the whole asio instance
            boost::asio::io_context& m_ioc;

            // hold all message to be sent to the remote side
            tsqueue<message<T>> m_qMessagesOut;

            // hold all messages that have been received from the remote side, note it a reference
            tsqueue<owned_message<T>>& m_qMessagesIn;


            // store the temprary msg because of incoming message are constructed asynchronously
            message<T> m_msgTemporaryIn;

            // The "owner" decides how some of the connection behaves
            owner m_nOwnerType = owner::server;

            uint32_t id = 0;

        };


    }
}
