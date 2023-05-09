#include "LanceNet/net/Acceptor.h"
#include "LanceNet/net/EventLoop.h"
#include <LanceNet/base/unix_wrappers.h>
#include <iostream>
#include <netdb.h>

using namespace std;
using namespace LanceNet;
using namespace net;

int main()
{
    int conn_fd = Open_clientfd("localhost", 2345, false);

    char buf[32];
    int n;
    while( (n = Read(conn_fd, buf, sizeof(buf))) > 0)
    {
        cout << buf << endl;
    }
    return 0;
}

