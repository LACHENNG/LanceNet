


#include "LanceNet/base/Thread.h"
#include "LanceNet/base/Time.h"
#include "LanceNet/net/EventLoop.h"
#include "LanceNet/net/FdChannel.h"
#include "LanceNet/base/Logging.h"

#include <bits/types/struct_itimerspec.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>

using namespace LanceNet;
using namespace LanceNet::net;
using namespace LanceNet::base;

static int timerfd;

void timeout(TimeStamp receiveTime)
{
    LOG_INFOC << "TimeOut " << receiveTime.toFmtString();

    uint64_t __;
    read(timerfd, &__, sizeof(uint64_t));
}

int createTimerFd(int period)
{
    timerfd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);

    itimerspec timerspec;
    timerspec.it_interval.tv_nsec = 0;
    timerspec.it_interval.tv_sec = 2;

    timerspec.it_value.tv_sec = 3;
    timerspec.it_value.tv_nsec = 0;

    timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &timerspec,  NULL);
    LOG_INFOC << "Timer fd created";
    return timerfd;
}

FdChannel* g_channel;

void thread_func()
{
    g_channel->setReadCallback(timeout);
    g_channel->enableReading();
}

// test EventLoop
int main(int argc, char* argv[])
{
    EventLoop loop;

    int timer_fd = createTimerFd(1);

    g_channel = new FdChannel(&loop, timer_fd);
    Thread thread(thread_func);
    thread.start();
    loop.StartLoop();

    thread.join();
    return 0;
}
