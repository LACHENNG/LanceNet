#include "LanceNet/base/ThisThread.h"
#include <pthread.h>
#include <stdio.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/base/Logging.h>
#include <LanceNet/base/Thread.h>
#include <unistd.h>

void threadFunc()
{
    
    LanceNet::net::EventLoop loop;
    loop.StartLoop();
}

int main()
{
    printf("main(): pid = %d, tid = %d", getpid(), LanceNet::ThisThread::Gettid());

    LanceNet::net::EventLoop loop;

    LanceNet::base::Thread thread(threadFunc);
    thread.start();

    loop.StartLoop();
    pthread_exit(NULL);
    return 0;
}
