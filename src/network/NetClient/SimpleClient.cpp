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

        std::cout << "Call PingServer Function \n";
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
            std::cout << "Client Connected, The incoming message is: " << c.Incoming().empty() << std::endl;
            if(!c.Incoming().empty()){
                std::cout << "Incoming Message have values  !!!!" << std::endl;
                auto msg = c.Incoming().pop_front().msg;
                
                std::cout << "\n msg : " << msg << std::endl;

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
