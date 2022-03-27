#include <iostream>
#include "../NetCommon/tplayn_net.hpp"


enum class CustomMsgTypes : uint32_t{
    FireBullet,
    MovePlayer
};

class CustomClient : public tplayn::net::client_interface<CustomMsgTypes>{
public:
    bool FireBullet(float x, float y){
        tplayn::net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::FireBullet;
        msg << x << y;
        Send(msg);
    }
};

int main(){
    CustomClient c;
    c.Connect("localhost", 3333);

    return 0;
}
