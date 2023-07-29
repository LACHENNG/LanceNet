#include "chat_server.h"
#include "LanceNet/net/EventLoop.h"

using namespace LanceNet;
using namespace net;
int main(int argc, const char* argv[])
{
    if(argc > 1){
        EventLoop loop;
        ChatServer chatServer(&loop, atoi(argv[1]));
        chatServer.start();
        loop.StartLoop();
    }
    else{
        printf("Usage: %s port\n", argv[0]);
    }

    return 0;
}
