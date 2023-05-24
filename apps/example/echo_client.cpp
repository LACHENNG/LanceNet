#include "LanceNet/base/Time.h"
#include "LanceNet/base/unix_wrappers.h"
#include "LanceNet/net/Buffer.h"
#include "LanceNet/net/EventLoop.h"
#include "LanceNet/net/TcpConnection.h"
#include <LanceNet/net/TcpServer.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace LanceNet;
using namespace net;


int main(int argc, char* argv[])
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

