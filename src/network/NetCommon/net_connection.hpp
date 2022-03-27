#pragma once

#include "net_common.hpp"
#include "net_tsqueue.hpp"
#include "net_message.hpp"

namespace tplayn
{
    namespace net
    {
        template<typename T>
        class connection : public std::enable_shared_from_this<connection<T>>{
        public:
            connection(){}

            virtual ~connection() {}
            
        public:
            bool ConnectToServer();
            bool DisConnect();
            bool IsConnected() const;

        public:
            bool send(const message<T>& msg);

        protected:
            // unique socket to remote
            boost::asio::ip::tcp::socket m_socket;

            // this context is shared with the whole asio instance
            boost::asio::io_context& m_ioc;

            // hold all message to be sent to the remote side
            tsqueue<message<T>> m_qMessagesOut;

            // hold all messages that have been received from the remote side, note it a reference
            tsqueue<owned_message<T>>& m_qMessageIn;

        };


    }
}
