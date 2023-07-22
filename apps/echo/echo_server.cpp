// Author : Lance @ nwpu
// echo_server that echo client data
#include "LanceNet/base/Logging.h"
#include "LanceNet/base/Time.h"
#include "LanceNet/net/Buffer.h"
#include "LanceNet/net/EventLoop.h"
#include "LanceNet/net/TcpConnection.h"
#include <LanceNet/net/TcpServer.h>

#include <iostream>
#include <unistd.h>

using namespace LanceNet;
using namespace net;
using namespace std;
using TcpConnectionPtr = TcpServer::TcpConnectionPtr;

EventLoop* gloop;


void OnMessage(const TcpConnection::TcpConnectionPtr& conn, Buffer* buffer, TimeStamp ts)
{
    //echo
    // std::string res = buffer->retrieveAllAsString();
    // conn->send(res);

    conn->send(buffer);
}


void OnConnection(const TcpConnectionPtr& connPtr, int conn_fd, const SA_IN* peer_addr)
{
    // cout << "Got connection from" << connPtr->peerIpPort() << endl;
}


int main()
{
    Logger::setLogLevel(Logger::LogLevel::DEBUG);
    gloop = new EventLoop;

    TcpServer echo_server(gloop, 3456, 8); // 4 threads
    echo_server.start();

    echo_server.setOnNewConnectionCb(OnConnection);
    echo_server.setOnMessageCb(OnMessage);

    gloop->StartLoop();
    return 0;
}

