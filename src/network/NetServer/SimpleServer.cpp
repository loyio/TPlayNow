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
        std::cout << "Get Message " << std::endl;
        std::cout << "When I get Message " << std::endl;
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
