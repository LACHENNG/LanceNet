#include <LanceNet/base/Time.h>
#include <LanceNet/base/numberConvertingAlogo/integer2cstr.h>

#include <chrono>
#include <assert.h>

namespace LanceNet
{

TimeStamp::TimeStamp()
  : time_point_(time_point<high_resolution_clock>::min())
{
}

TimeStamp::TimeStamp(Timepoint timepoint)
  : time_point_(timepoint)
{
}

int64_t TimeStamp::cvt2MicroSecSinceEpoch()
{
    auto duration = time_point_.time_since_epoch();
    int64_t microSecSinceEpoch = duration_cast<std::chrono::microseconds>(duration).count();
    return microSecSinceEpoch;
}

int64_t TimeStamp::cvt2NanoSecSinceEpoch()
{
    auto duration = time_point_.time_since_epoch();
    int64_t NanoSecSinceEpoch = duration_cast<std::chrono::nanoseconds>(duration).count();
    return NanoSecSinceEpoch;
}
std::string TimeStamp::toString()
{
    assert(isValid());
    int64_t microSecSinceEpoch = cvt2MicroSecSinceEpoch();
    int64_t secs = microSecSinceEpoch / MicroSecondsPerSecond;
    int64_t microSecs = microSecSinceEpoch % MicroSecondsPerSecond;

    char buf[32];
    snprintf(buf, sizeof(buf)-1, "%" PRId64 "%.06" PRId64 "", secs, microSecs);
    return buf;
}

std::string TimeStamp::toFmtString(bool showMicroSec)
{
    char buf[64] = {0};

    std::time_t seconds = system_clock::to_time_t(time_point_);
    struct tm tm_time;
    tm_time = *std::localtime(&seconds);

    if(showMicroSec)
    {
        int microSecs = cvt2MicroSecSinceEpoch() % MicroSecondsPerSecond;
        snprintf(buf, sizeof(buf) - 1, "%4d%02d%02d %02d:%02d:%02d.%06d",
                tm_time.tm_year+1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                microSecs);
    }else {
        snprintf(buf, sizeof(buf) - 1, "%4d%02d%02d %02d:%02d:%02d",
                tm_time.tm_year+1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
    return buf;
}

TimeStamp TimeStamp::illegal()
{
    return TimeStamp(system_clock::time_point::min());
}

TimeStamp TimeStamp::now()
{
    return TimeStamp(system_clock::now());
}

TimeStamp TimeStamp::addSeconds(TimeStamp basetime, double secs)
{
    auto newTimepoint = basetime.getTimePoint() + std::chrono::nanoseconds(static_cast<long>(secs*1e9));
    return TimeStamp(newTimepoint);
}

int64_t TimeStamp::getDiffInNanoSeconds(TimeStamp t1, TimeStamp t2)
{
    auto duration = t2.getTimePoint() - t1.getTimePoint();
    int64_t nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
   return nanoseconds;
}

/* GET ELAPSED TIME IN second PRECSION */
int64_t getElapsedSec(TimeStamp start, TimeStamp end){
    return getElapsedNanoSec(start, end) * 1e-9;
}

/* GET ELAPSED TIME IN millisecond PRECSION */
int64_t getElapsedMilliSec(TimeStamp start, TimeStamp end){
    return getElapsedNanoSec(start, end) * 1e-6;
}
/* GET ELAPSED TIME IN microsecond PRECSION */
int64_t getElapsedMicroSec(TimeStamp start, TimeStamp end)
{
    return getElapsedNanoSec(start, end) * 1e-3;
}

/* get elapsed time in  nanosecond precision */
int64_t getElapsedNanoSec(TimeStamp start, TimeStamp end){
    auto dur = end.getTimePoint() - start.getTimePoint();
    return duration_cast<nanoseconds>(dur).count();
}

} // namespace LanceNet
