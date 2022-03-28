#include <iostream>
#include "../NetCommon/tplayn_net.hpp"

enum class CustomMsgTypes : uint32_t
{
    ServerAccept, ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage,
};

class CustomServer : public tplayn::net::server_interface<CustomMsgTypes>{
public:
    CustomServer(uint32_t nPort) : tplayn::net::server_interface<CustomMsgTypes>(nPort){
    }

protected:
    virtual bool OnClientConnect(std::shared_ptr<tplayn::net::connection<CustomMsgTypes>> client){
        return true;
    }

    virtual void OnClientDisconnect(std::shared_ptr<tplayn::net::connection<CustomMsgTypes>> client){
    }


    virtual void OnMessage(std::shared_ptr<tplayn::net::connection<CustomMsgTypes>> client, tplayn::net::message<CustomMsgTypes> msg){
    }


};


int main(){
    CustomServer server(50000);
    server.Start();

    while(1){
        server.Update();
    }
    return 0;
}
