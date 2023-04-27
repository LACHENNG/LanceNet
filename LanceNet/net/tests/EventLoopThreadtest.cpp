

#include "LanceNet/net/EventLoopThread.h"
#include "LanceNet/net/EventLoop.h"
#include "LanceNet/base/Logging.h"

using namespace LanceNet;
using namespace LanceNet::net;

int main()
{
    LOG_INFOC << "main thread: " << ThisThread::Gettid();

    LanceNet::net::EventLoopThread eventLoopthread;

    auto EventLoop = eventLoopthread.StartLoop();
    // this should fail
    EventLoop->assertInEventLoopThread();


    return 0;
}
