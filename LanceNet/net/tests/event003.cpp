
#include "LanceNet/base/ThisThread.h"
#include <pthread.h>
#include <stdio.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/base/Logging.h>
#include <LanceNet/base/Thread.h>
#include <unistd.h>

LanceNet::net::EventLoop loop;

void threadFunc()
{
    LanceNet::net::EventLoop loop;
    loop.StartLoop();
}

// Test invoke loop.StartLoop in a different thread from which it was created
// which should cause a FATAL error
int main()
{
    LOG_INFOC << "main (): pid = " << getpid() << ", tid = " << LanceNet::ThisThread::Gettid();

    LanceNet::base::Thread thread(threadFunc);
    thread.start();

    LanceNet::net::EventLoop loop2;
    loop2.StartLoop();

    thread.join();
    return 0;

}
