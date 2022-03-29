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


namespace tplayn
{
    namespace net
    {
        template<typename T>
        class tsqueue{
        public:
            tsqueue() = default;
            tsqueue(const tsqueue<T>&) = delete;
            virtual ~tsqueue() { clear();}


        public:
            // front of queue
            const T& front(){
                std::scoped_lock lock(muxQueue);
                return deqQueue.front();
            }

            // Returns and maintains item at back of Queue 
            const T& back(){
                std::scoped_lock lock(muxQueue);
                return deqQueue.back();
            }

            // push items to the back
            void push_back(const T& item){
                std::scoped_lock lock(muxQueue);
                deqQueue.emplace_back(std::move(item));

                std::unique_lock<std::mutex> ul(muxBlocking);
                cvBlocking.notify_one();
            }

            // push items to the front
            void push_front(const T& item){
                std::scoped_lock lock(muxQueue);
                deqQueue.emplace_front(std::move(item));

                std::unique_lock<std::mutex> ul(muxBlocking);
                cvBlocking.notify_one();
            }


            // Return true if queue is empty
            bool empty(){
                std::scoped_lock lock(muxQueue);
                return deqQueue.empty();
            }


            // Return the numbers of items on queue
            size_t count(){
                std::scoped_lock lock(muxQueue);
                return deqQueue.size();
            }

            // Clears queue
            void clear(){
                std::scoped_lock lock(muxQueue);
                deqQueue.clear();
            }

            // remove and get the front of queue
            T pop_front(){
                std::scoped_lock lock(muxQueue);
                auto t = std::move(deqQueue.front());
                deqQueue.pop_front();
                return t;
            }

            // remove and get the back of queue
            T pop_back(){
                std::scoped_lock lock(muxQueue);
                auto t = std::move(deqQueue.back());
                deqQueue.pop_back();
                return t;
            }

            void wait(){
                while(empty()){
                    std::unique_lock<std::mutex> ul(muxBlocking);
                    cvBlocking.wait(ul);
                }
            }


        protected:
            std::mutex muxQueue;
            std::deque<T> deqQueue;
            std::condition_variable cvBlocking;
            std::mutex muxBlocking;
        };
    }
}
