


#include "LanceNet/net/EventLoop.h"
#include "LanceNet/net/FdChannel.h"
#include "LanceNet/base/Logging.h"

#include <bits/types/struct_itimerspec.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>

using namespace LanceNet;
using namespace LanceNet::net;

static int timerfd;

void timeout()
{
    LOG_INFOC << "TimeOut ";

    uint64_t __;
    read(timerfd, &__, sizeof(uint64_t));
}

int createTimerFd(int period)
{
    timerfd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);

    itimerspec timerspec;
    timerspec.it_interval.tv_nsec = 0;
    timerspec.it_interval.tv_sec = 2;

    timerspec.it_value.tv_sec = 5;
    timerspec.it_value.tv_nsec = 0;

    timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &timerspec,  NULL);
    LOG_INFOC << "Timer fd created";

    return timerfd;
}

// test FdChannel
int main(int argc, char* argv[])
{
    EventLoop loop;

    int timer_fd = createTimerFd(1);

    FdChannel channel(&loop, timer_fd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    loop.StartLoop();
    return 0;
}
