
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
    loop.StartLoop();
}

// Test invoke loop.StartLoop in a different thread from which it was created
// which should cause a FATAL error
int main()
{
    LOG_INFOC << "main (): pid = " << getpid() << ", tid = " << LanceNet::ThisThread::Gettid();

    LanceNet::base::Thread thread(threadFunc);
    thread.start();

    thread.join();
    return 0;
}
