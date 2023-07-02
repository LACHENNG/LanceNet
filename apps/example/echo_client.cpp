// Author: Lance @ nwpu
// echo_client that read user inputs and send to server

#include "LanceNet/base/CountDownLatch.h"
#include "LanceNet/base/Mutex.h"
#include "LanceNet/base/Thread.h"
#include "LanceNet/base/Time.h"
#include "LanceNet/base/unix_wrappers.h"
#include "LanceNet/net/Buffer.h"
#include "LanceNet/net/EventLoop.h"
#include <LanceNet/net/TcpConnection.h>
#include <LanceNet/net/TcpServer.h>
#include <LanceNet/net/TcpClient.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>

#include <iostream>
using namespace std;
using namespace LanceNet;
using namespace net;

using TcpConnectionPtr = TcpClient::TcpConnectionPtr;

TcpConnectionPtr tcpconnection;
MutexLock mutex;

// wait for tcpconnectionPtr ready
CountDownLatch latch(1);

void OnMessage(const TcpConnectionPtr& connection, Buffer* buf, TimeStamp ts)
{
    string message = buf->retrieveAllAsString();
    cout << "received: '" << message << "' from server" << endl;
}

void OnConnection(TcpConnectionPtr connPtr, int conn_fd, const SA_IN* peer_addr)
{
    {
    MutexLockGuard lock(mutex);
    tcpconnection = connPtr;
    }
    latch.countdown();
}

void OnDisconnection(TcpConnectionPtr connPtr, int conn_fd, const SA_IN* peer_addr)
{
    MutexLockGuard lock(mutex);
    tcpconnection.reset();
}


void getConsoleInputAndSend()
{

    latch.wait();

    std::string str;
    while(std::cin >> str)
    {
        TcpConnectionPtr connection;
        {
            MutexLockGuard lock(mutex);
            if(tcpconnection){
                connection = tcpconnection;
            }
        }
        if(!connection){
            break;
        }
        connection->send(str);
    }
}

int main(int argc, char* argv[])
{
    if(argc != 3){
        printf("usage: ./xxx [port] [server name]\n");
        return 0;
    }

    int port = stoi(std::string(argv[1]));

    base::Thread thread(getConsoleInputAndSend);
    thread.start();

    EventLoop* loop = new EventLoop();
    TcpClient client(port, argv[2], loop);
    client.setOnConnectionCallback(OnConnection);
    client.setOnMessageCallback(OnMessage);
    client.setOnDisconnectedCb(OnDisconnection);

    client.start();
    loop->StartLoop();

    return 0;
}


int main1(int argc, char* argv[])
{
    if(argc != 2){
        printf("usage: ./xxx [port]\n");
        return 0;
    }
    int port = stoi(std::string(argv[1]));
    int connfd = Open_clientfd("localhost", port, false);

    std::string str;
    char buf[512];

    while(std::cin >> str)
    {
        Write(connfd, str.data(), str.size());
        int n = Read(connfd, buf, 512);
        if(n < 512) buf[n] = 0;
        else buf[512-1] = 0;

        printf("%s", buf);
    }

    return 0;
}

