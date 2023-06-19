#include "LanceNet/base/Time.h"
#include <LanceNet/net/EventLoopPool.h> 
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/base/Thread.h>

#include <stdio.h>
#include <unistd.h>

using namespace LanceNet;
using namespace LanceNet::net;

void print(EventLoop* p = NULL)
{
    printf("main(): pid = %d, tid = %d, loop = %p\n",
            getpid(), ThisThread::Gettid(), p);
}

void init(EventLoop* p)
{
    printf("init(): pid = %d, tid = %d, loop = %p\n",
            getpid(), ThisThread::Gettid(), p);
}

int main()
{
    print();

    EventLoop loop;
    loop.runAt(TimeStamp::now() + 11, std::bind(&EventLoop::quit, &loop));

    {
        printf("Single thread %p:\n", &loop);
        EventLoopPool model(&loop);
        model.setThreadNum(0);
        model.start();
        assert(model.getNextLoop() == &loop);
        assert(model.getNextLoop() == &loop);
        assert(model.getNextLoop() == &loop);
    }

    {
        printf("another thread %p:\n", &loop);
        EventLoopPool model(&loop);
        model.setThreadNum(1);
        model.start();
        EventLoop* nextLoop = model.getNextLoop();
        nextLoop->runAt(TimeStamp::now() + 2, std::bind(print, nextLoop));

        assert(model.getNextLoop() != &loop);
        assert(model.getNextLoop() == nextLoop);
        assert(model.getNextLoop() == nextLoop);
        ::sleep(3);
    }

    {
        printf("Thread threads %p:\n", &loop);
        EventLoopPool model(&loop);
        model.setThreadNum(3);
        model.start();
        EventLoop* nextLoop = model.getNextLoop();
        assert(&loop != nextLoop);
        assert(model.getNextLoop() != nextLoop);
        assert(model.getNextLoop() != nextLoop);
    }

    loop.StartLoop();
    return 0;
}


