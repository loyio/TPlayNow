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

            // push items to the back
            void push_back(const T& item){
                std::scoped_lock lock(muxQueue);
                deqQueue.emplace_back(std::move(item));
            }

            // push items to the front
            void push_front(const T& item){
                std::scoped_lock locak(muxQueue);
                deqQueue.emplace_front(std::move(item));
            }


            // Return true if queue is empty
            bool empty(){
                std::scoped_lock lock(muxQueue);
                deqQueue.empty();
            }


            // Return the numbers of items on queue
            size_t count(){
                std::scoped_lock lock(muxQueue);
                deqQueue.size();
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


        protected:
            std::mutex muxQueue;
            std::deque<T> deqQueue;
            std::condition_variable cvBlocking;
            std::mutex muxBlocking;
        };
    }
}
