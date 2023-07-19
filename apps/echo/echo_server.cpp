// Author : Lance @ nwpu
// echo_server that echo client data
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

void OnMessage(TcpConnection::TcpConnectionPtr conn, Buffer* buffer, TimeStamp ts)
{
    //echo
    std::string res = buffer->retrieveAllAsString();
    cout << "server received: " << res << endl;
    conn->send(res);
}


void OnConnection(TcpConnectionPtr connPtr, int conn_fd, const SA_IN* peer_addr)
{

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &peer_addr->sin_addr, ip, sizeof(ip));
    int port = ntohs(peer_addr->sin_port);

    cout << "Got connection from" << ip << ":" << port << endl;
}


int main()
{
    gloop = new EventLoop;

    TcpServer echo_server(gloop, 3456);
    echo_server.start();

    echo_server.setOnNewConnectionCb(OnConnection);
    echo_server.setOnMessageCb(OnMessage);

    gloop->StartLoop();

    return 0;
}

