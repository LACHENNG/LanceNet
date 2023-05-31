#include "LanceNet/base/Time.h"
#include "LanceNet/net/Buffer.h"
#include "LanceNet/net/EventLoop.h"
#include "LanceNet/net/TcpConnection.h"
#include <LanceNet/net/TcpServer.h>
#include <unistd.h>

using namespace LanceNet;
using namespace net;

EventLoop* gloop;

void OnMessage(TcpConnection::TcpConnectionPtr conn, Buffer* buffer, TimeStamp ts)
{
    ::sleep(4);
    //echo
    std::string res = buffer->retrieveAllAsString();
    conn->send(res);
}


int main()
{
    gloop = new EventLoop;

    TcpServer echo_server(gloop, 3456);
    echo_server.start();

    echo_server.setMessageCb(OnMessage);
    gloop->StartLoop();

    return 0;
}

