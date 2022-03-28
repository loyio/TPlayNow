#include <iostream>
#include "../NetCommon/tplayn_net.hpp"


enum class CustomMsgTypes : uint32_t{
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage,
};

class CustomClient : public tplayn::net::client_interface<CustomMsgTypes>{

};

int main(){
    CustomClient c;
    c.Connect("127.0.0.1", 50000);
    return 0;
}
