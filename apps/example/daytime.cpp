#include "LanceNet/base/Time.h"
#include "LanceNet/net/Buffer.h"
#include "LanceNet/net/EventLoop.h"
#include "LanceNet/net/TcpConnection.h"
#include "LanceNet/base/Logging.h"
#include <LanceNet/net/TcpServer.h>
#include <unistd.h>

using namespace LanceNet;
using namespace net;

EventLoop* gloop;

void OnConnection(TcpConnection::TcpConnectionPtr conn, int fd, const SA_IN* peer)
{
   char name[32];
   char port[32];
   getnameinfo((SA*)peer, sizeof(SA), name, sizeof(name), port, sizeof(port),  NI_NUMERICHOST | NI_NUMERICHOST);
   LOG_INFOC << "Server got connection name = " << name << " port = " << port;

   conn->send(TimeStamp::now().toFmtString() + "\n");
   conn->shutdown();
   LOG_INFOC << "sleep";
   sleep(4);
   LOG_INFO << "done with sleep";
}

int main()
{
    gloop = new EventLoop;

    TcpServer daytime_server(gloop, 3456);
    daytime_server.start();

    daytime_server.setNewConnectionCb(OnConnection);
    gloop->StartLoop();

    return 0;
}

