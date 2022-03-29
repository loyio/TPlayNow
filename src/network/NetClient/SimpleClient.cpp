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
#include "../NetCommon/tplayn_net.hpp"
#include <ncurses.h>


enum class CustomMsgTypes : uint32_t{
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage,
};

class CustomClient : public tplayn::net::client_interface<CustomMsgTypes>{
public:
    void PingServer(){

        tplayn::net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerPing;

        // please check your server and client is same system
        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

        msg << timeNow;
        
        Send(msg);
    }


};

int main(){
    // Keyboard event
    initscr();
    cbreak();
    scrollok(stdscr, TRUE);
    noecho();
    keypad(stdscr, TRUE);

    CustomClient c;
    c.Connect("127.0.0.1", 60000);


    bool bQuit = false;
    while(!bQuit){
        

        int ch;
        ch = getch();
        
        if(ch == 49){
            c.PingServer();
        }
        else if(ch == 'q'){
            bQuit = true;
        }
        
        if(c.IsConnected()){
            if(!c.Incoming().empty()){
                auto msg = c.Incoming().pop_front().msg;
                switch(msg.header.id){
                    case CustomMsgTypes::ServerAccept:
                        std::cout << "Server Accepted Connection" << std::endl;
                        break;
                    case CustomMsgTypes::ServerPing:{
                        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                        std::chrono::system_clock::time_point timeThen;
                        msg >> timeThen;
                        std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << std::endl;
                        break;
                    }
                    case CustomMsgTypes::MessageAll:{
                        break;
                    }
                    case CustomMsgTypes::ServerMessage:{
                        uint32_t clientID;
                        msg >> clientID;
                        std::cout << "Hello from [" << clientID << "]" << std::endl;
                        break;
                    }
                }
            }
        }else{
           std::cout << "Server Down" << std::endl;
           bQuit = true;
        }
    }
    return 0;
}
