// Author: Lance @ nwpu
#ifndef LanceNet_NET_EVENTLOOPPOOL_H_
#define LanceNet_NET_EVENTLOOPPOOL_H_
#include "LanceNet/base/noncopyable.h"
#include "LanceNet/net/EventLoopThread.h"
#include <vector>

namespace LanceNet
{
namespace net
{

class EventLoop;

class EventLoopPool : noncopyable
{
public:
    EventLoopPool(EventLoop* mainloop);
    ~EventLoopPool(); // force out-line dtor
    void setThreadNum(int numThreads);
    void start();
    EventLoop* getNextLoop();
    EventLoop* getMainLoop() const { return mainLoop_;}

private:
    bool started_;
    int threadNum_;
    int next_;
    EventLoop* mainLoop_;
    using EventLoopThreadPtr = std::shared_ptr<EventLoopThread>;
    std::vector<EventLoopThreadPtr> threads_;
    std::vector<EventLoop*> loops_;
};

} // namespace net 
} // namespace LanceNet;


#endif // LanceNet_NET_EVENTLOOPPOOL_H_
