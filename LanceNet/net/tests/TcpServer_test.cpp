#include <LanceNet/net/FdChannel.h>
#include "LanceNet/base/Time.h"
#include "LanceNet/net/EventLoop.h"
#include "LanceNet/net/TcpConnection.h"
#include <LanceNet/net/TcpServer.h>
#include <LanceNet/base/Logging.h>
#include <pthread.h>

using namespace LanceNet;
using namespace net;

std::atomic_int32_t totalBytesReceived{0};

EventLoop loop;

using TcpConnectionPtr = TcpConnection::TcpConnectionPtr;
void OnMessage(const TcpConnectionPtr& conn, Buffer* buf, TimeStamp receiveTime)
{
    totalBytesReceived += buf->readableBytes();
    LOG_INFOC << "Server Received Message : " << buf->retrieveAllAsString();

    LOG_INFOC << "total received bytes: " << totalBytesReceived; 
}

void OnConnectionEstablished(TcpConnection::TcpConnectionPtr connPtr, int conn_fd, const SA_IN* peer)
{
   char name[32];
   char port[32];
   getnameinfo((SA*)peer, sizeof(SA), name, sizeof(name), port, sizeof(port),  NI_NUMERICHOST | NI_NUMERICHOST);

   LOG_INFOC << "Server got connection name = " << name << " port = " << port;
}

int main()
{
    TcpServer tcpserver(&loop, 2345);

    tcpserver.setMessageCb(OnMessage);
    tcpserver.setNewConnectionCb(OnConnectionEstablished);
    tcpserver.start();

    loop.StartLoop();
    pthread_exit(0);
    // return 0;
}
