#include <assert.h>

#include "LanceNet/base/Time.h"
#include <LanceNet/net/Timer.h>
#include <LanceNet/base/Logging.h>
namespace LanceNet
{
namespace net
{

std::atomic_int64_t Timer::numTimersCreated{0};

Timer::Timer(TimeStamp expiration, Callback onAlarmCb, bool repeat, double intervalSecs)
  : timeExpiration_(expiration),
    onAlarmCb_(onAlarmCb),
    repeat_(repeat),
    intervalSecs_(intervalSecs),
    id_(getAndIncreaseId())
{

    LOG_INFOC << "Timer::Timer() with id= "  << id_;
}

Timer::~Timer()
{
    LOG_INFOC << "~Timer() with id= "  << id_;
    numTimersCreated--;
}

void Timer::OnExpiration()
{
    assert(TimeStamp::now() >= timeExpiration_);
    LOG_INFOC << "Timer with id = " << id() << " OnExpiration()";
    onAlarmCb_();
}

TimeStamp Timer::expiration() const
{
    return timeExpiration_;
}

void Timer::resetExpiration(TimeStamp newExpiration)
{
    timeExpiration_ = newExpiration ;
}

void Timer::restartTimer()
{
    if(isRepeat()){
        resetExpiration(TimeStamp::now() + intervalSecs_);
    }
    else{
        resetExpiration(TimeStamp::illegal());
    }
}

int64_t Timer::id()
{
    return id_;
}

int64_t Timer::getAndIncreaseId()
{
    return ++Timer::numTimersCreated;
}

bool Timer::isRepeat() const
{
    return repeat_;
}

double Timer::intervalSecs() const
{
    return intervalSecs_;
}


} // net
} // namespace LanceNet
