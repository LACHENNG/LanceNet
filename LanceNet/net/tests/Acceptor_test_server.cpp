#include "LanceNet/net/EventLoop.h"
#include "LanceNet/net/FdChannel.h"
#include <LanceNet/net/Acceptor.h>
#include <LanceNet/base/Logging.h>
#include <netdb.h>
#include <string>
#include <unistd.h>
using namespace LanceNet;
using namespace net;

void onConnection(int peer_socket, const SA_IN* peer_sockaddr)
{
    char name[32];
    char port[32];
    getnameinfo((SA*)peer_sockaddr, sizeof(SA), name, sizeof(name), port, sizeof(port),  NI_NUMERICHOST | NI_NUMERICHOST);

    LOG_INFOC << "Server got connection name = " << name << " port = " << port;

    std::string message = std::string("Server says: hello ") +  std::string(name);
    Write(peer_socket, message.c_str(), message.size() + 1);

    Close(peer_socket);
}

int main()
{
    EventLoop loop;

    Acceptor accptor(&loop,2345);
    accptor.setOnConnectionCallback(onConnection);

    loop.StartLoop();
    return 0;
}
