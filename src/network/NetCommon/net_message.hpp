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

namespace tplayn{
    namespace net{
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
            }
        };
    }
}
