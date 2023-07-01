// Autor: Lance @ nwpu
#ifndef LanceNet_NET_CONNECCTOR_H
#define LanceNet_NET_CONNECCTOR_H

#include "LanceNet/base/StringPiece.h"
#include "LanceNet/base/noncopyable.h"
#include <functional>

namespace LanceNet
{
namespace net
{

class EventLoop;
// A connector try to connect to A TcpServer
// and A callback with parameter int socket is called
// A back-off retry is performed when connection fails
//

class Connector : noncopyable
{
public:
    using ConnectionCallback = std::function<void(int socket)>;

    Connector(EventLoop* loop, int port, std::string hostname);
    ~Connector();

    void setConnectionCallback(ConnectionCallback cb);

    void start();
    void stop();
    void restart();

private:
    int port_;
    std::string hostname_;
    double next_retryInterval_; // in seconds
    ConnectionCallback cb_;
    bool started_;
    bool stop_;
    EventLoop* loop_;
};

} // namespace LanceNet
} // namespace net

#endif
