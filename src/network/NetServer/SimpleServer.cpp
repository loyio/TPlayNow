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
#include <iostream>
#include "../NetCommon/tplayn_net.hpp"

enum class CustomMsgTypes : uint32_t
{
    ServerAccept, 
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage,
};

class CustomServer : public tplayn::net::server_interface<CustomMsgTypes>{
public:
    CustomServer(uint16_t nPort) : tplayn::net::server_interface<CustomMsgTypes>(nPort){
    }

protected:
    virtual bool OnClientConnect(std::shared_ptr<tplayn::net::connection<CustomMsgTypes>> client){
        tplayn::net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerAccept;
        client->Send(msg);
        return true;
    }

    virtual void OnClientDisconnect(std::shared_ptr<tplayn::net::connection<CustomMsgTypes>> client){
        std::cout << "Removing client [" << client->GetID() << "]" << std::endl;
    }


    virtual void OnMessage(std::shared_ptr<tplayn::net::connection<CustomMsgTypes>> client, tplayn::net::message<CustomMsgTypes>& msg){
        switch(msg.header.id){
            case CustomMsgTypes::ServerPing:
            {
                std::cout << "[" << client->GetID() << "]: Server Ping\n";

                // bounce message back to client
                client->Send(msg);
            }
            break;

            case CustomMsgTypes::MessageAll:
            {
                std::cout << "[" << client->GetID() << "]: Message All\n";

                // bounce message back to client
                tplayn::net::message<CustomMsgTypes> msg;
                msg.header.id = CustomMsgTypes::ServerMessage;
                msg << client->GetID();
                MessageAllClients(msg, client);
            }
            break;

        }
    }


};


int main(){
    CustomServer server(60000);
    server.Start();

    while(1){
        server.Update(100, true);
    }
    return 0;
}
