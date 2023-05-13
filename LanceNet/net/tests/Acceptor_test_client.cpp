#include "LanceNet/net/Acceptor.h"
#include "LanceNet/net/EventLoop.h"
#include <LanceNet/base/unix_wrappers.h>
#include <iostream>
#include <netdb.h>

using namespace std;
using namespace LanceNet;
using namespace net;

int connect()
{
    int conn_fd = Open_clientfd("localhost", 2345, false);
//    int conn_fd2 = Open_clientfd("localhost", 2345, false);

    char buf[32];
    int n;
    while( (n = Read(conn_fd, buf, sizeof(buf))) > 0)
    {
        cout << buf << endl;
    }
    return 0;
}

int main()
{
    connect();
    return 0;
}
