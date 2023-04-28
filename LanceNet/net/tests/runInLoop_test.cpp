
#include "LanceNet/base/ThisThread.h"
#include "LanceNet/base/Thread.h"
#include "LanceNet/net/EventLoopThread.h"
#include "LanceNet/net/EventLoop.h"
#include "LanceNet/net/FdChannel.h"
#include "LanceNet/base/Logging.h"

#include <bits/types/struct_itimerspec.h>
#include <memory>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>
#include <unistd.h>

using namespace LanceNet;
using namespace LanceNet::net;
using namespace LanceNet::base;

EventLoopThread* g_loop;

std::shared_ptr<EventLoop> loopPtr;

void thread_func()
{
    auto tid = ThisThread::Gettid();
    LOG_INFOC << "Pending Funcions to EventLoop at Thread : " << tid;
    loopPtr->runInLoop([tid](){
        LOG_INFOC << "thread_func(): the pending Funcions is pend at thread: " << tid << " and is acutally exectued at thread: " << ThisThread::Gettid();
    });
}

void thread_func2()
{
    auto tid = ThisThread::Gettid();
    loopPtr->runInLoop([tid](){
        LOG_INFOC << "thread_func2(): the pending Funcions is pend at thread: " << tid << " and is acutally exectued at thread: " << ThisThread::Gettid();
    });
    // loopPtr->runInLoop([tid](){printf("thread_func2(): The pending Funcions is pend at thread: %d and is acutally exectued at thread: %d\n", tid, ThisThread::Gettid());});
}

int main(int argc, char* argv[])
{
    LOG_INFOC << "Main thread : " << ThisThread::Gettid();

    g_loop = new EventLoopThread();
    loopPtr = g_loop->StartLoop();
    assert(loopPtr);

    Thread thread(thread_func);
    Thread thread2(thread_func2);
    thread.start();
    thread2.start();

    thread.join();
    thread2.join();
    LOG_INFOC << "main returned";
    pthread_exit(NULL); 
    //return 0;
}
