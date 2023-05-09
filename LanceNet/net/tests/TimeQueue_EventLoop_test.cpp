#include "LanceNet/base/Time.h"
#include <LanceNet/net/EventLoopThread.h>
#include <LanceNet/net/EventLoop.h>
#include <LanceNet/base/Logging.h>
#include <pthread.h>
#include <atomic>
#include <LanceNet/net/Timer.h>

using namespace LanceNet;
using namespace LanceNet::net;

EventLoopThread loopThread;
std::atomic_int32_t ncalls{0};

void func()
{
    LOG_INFOC << "timerout func() called";
}

int main()
{
    auto loopPtr = loopThread.StartLoop();
    auto func = [&](){
        LOG_INFOC << "timerout func() called";
    };

    // runAt test
    TimeStamp when = TimeStamp::now() + 0.000001;

    // test expire at same time
    loopPtr->runAt(when, func);
    loopPtr->runAt(when, [&](){
        LOG_INFOC << "callback called " << ncalls;
    });

    // test expire at past time
    TimeStamp past = TimeStamp::now();
    loopPtr->runAt(past, [&](){
        LOG_INFOC << "callback called " << ncalls;
    });

    ////////////////////////////////////////////////////////////////////
    ///run every
    ////////////////////////////////////////////////////////////////////
    ///
    TimeStamp start = TimeStamp::now() + 2;
    LOG_INFOC << "-----begin run every at " << start.toFmtString() << "\n";

    loopPtr->runEvery(start, 0.5, func);
    loopPtr->runEvery(start, 1, func);


    LOG_INFOC << "-----begin run every at " << start.toFmtString() << "\n";

    pthread_exit(0);

    //return 0;
}



