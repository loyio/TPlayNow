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
#pragma once
#include "net_common.hpp"

namespace tplayn
{
    namespace net
    {
        // Message Header sent at start, templete allow up to use "Enum class" to ensure message valid
        template <typename T>
        struct message_header{
            T id{};
            uint32_t size = 0;
        };
         
        template <typename T>
        struct message {
            message_header<T> header{};
            std::vector<uint8_t> body;

            size_t size() const{
                return sizeof(message_header<T>) + body.size();
            }

            friend std::ostream& operator << (std::ostream& os, const message<T>& msg){
                os << "ID: " << int(msg.header.id) << " Size: " << msg.header.size;
                return os;
            }

            // pushes any pod-like data into the message buffer
            template<typename DataType>
            friend message<T>& operator << (message<T>& msg, const DataType& data){
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

                // Cache current size of vector by the size of the data being pushed
                size_t i = msg.body.size();

                // resize the vector
                msg.body.resize(msg.body.size() + sizeof(DataType));

                // Physically copy the data into the newly allocated vector space
                std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

                // Recalculate the message size
                msg.header.size = msg.size();

                return msg;
            }

            template<typename DataType>
            friend message<T>& operator >> (message<T>& msg, DataType& data){
                // Check that the type of the data being pushed is trivially copyable
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

                size_t i = msg.body.size() - sizeof(DataType);

                // copy the data from the vector to user variable
                std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

                // shrink the vector to remove read type, and reset end position
                msg.body.resize(i);

                msg.header.size = msg.size();

                return msg;
            }
        };


        // Connection
        template <typename T>
        class connection;


        template <typename T>
        struct owned_message{
            std::shared_ptr<connection<T>> remote = nullptr;
            message<T> msg;


            // string maker
            friend std::ostream& operator<<(std::ostream& os, const owned_message<T>& msg){
                os << msg.msg;
                return os;
            }
        };

    }
}
