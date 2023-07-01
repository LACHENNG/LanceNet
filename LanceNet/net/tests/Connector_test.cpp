#include "LanceNet/net/EventLoop.h"
#include <LanceNet/net/Connector.h>

using namespace LanceNet;
using namespace net;

EventLoop* loop = nullptr;

void connectCallback(int sockfd)
{
    printf("connected.\n");
    loop->quit();
}

int main()
{
    loop = new EventLoop();

    Connector connector(loop, 80, "127.0.0.1");
    connector.setConnectionCallback(connectCallback);
    connector.start();
    loop->StartLoop();
    return 0;
}
