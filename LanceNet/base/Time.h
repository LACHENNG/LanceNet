// Author: Lance(Lang Chen) @ nwpu
// Time related functionalities (eg. timestamp, time elapsed, print timestamp et.) with nanosecond precision
#ifndef LanceNet_BASE_TIME_H
#define LanceNet_BASE_TIME_H

#include <chrono>
#include <iomanip> // std::put_time
#include <string>

#define __STDC_FORMAT_MACROS 1
#include <cinttypes> // RPId 

#include <LanceNet/base/copyable.h>

namespace LanceNet
{

using namespace std::chrono;
using namespace std::chrono_literals;

class TimeStamp : copyable{
public:
    using Timepoint = time_point<system_clock>;
    static const int MicroSecondsPerSecond = 1e6;
    // ctors
    // construct a TimeStamp represents invalid time
    TimeStamp();
    // construct a TimeStamp at a specific time
    explicit TimeStamp(Timepoint timepoint);

    // to str
    // seconds.microsec
    std::string toString();
    // like 20230203 12:00:00.microsec
    std::string toFmtString(bool showMicroSec = true);

    // Factory
    // get a invalid timestamp
    static TimeStamp illegal();
    // get Timestamp of now
    static TimeStamp now();

    //arithmetics
    static TimeStamp addSeconds(TimeStamp basetime, double secs);
    static double getDiffInSeconds(TimeStamp t1, TimeStamp t2);

    // observer
    bool isValid(){ return !(time_point_ == Timepoint::min());}

    // getters
    Timepoint getTimePoint() const
    {
        return time_point_;
    }

private:
    // convert current time_point_ to micro/nano secs since epoch 
    int64_t cvt2MicroSecSinceEpoch();
    int64_t cvt2NanoSecSinceEpoch();

private:
    // most system clock impl use UTC time, but is not standardized until C++20  // system_clock is in nanosec precision
    Timepoint time_point_;
};

inline bool operator<(const TimeStamp lhs, const TimeStamp rhs)
{
    return lhs.getTimePoint() < rhs.getTimePoint();
}

inline bool operator==(const TimeStamp lhs, const TimeStamp rhs)
{
    return lhs.getTimePoint() == rhs.getTimePoint();
}

inline bool operator>=(const TimeStamp lhs, const TimeStamp rhs)
{
    return !(lhs.getTimePoint() < rhs.getTimePoint());
}

// Arithmetics
inline TimeStamp operator+(TimeStamp lhs, double offsetSeconds)
{
    return TimeStamp::addSeconds(lhs, offsetSeconds);
}

/* GET ELAPSED TIME IN second PRECSION */
int64_t getElapsedSec(TimeStamp start, TimeStamp end);
/* GET ELAPSED TIME IN millisecond PRECSION */
int64_t getElapsedMilliSec(TimeStamp start, TimeStamp end);
/* GET ELAPSED TIME IN microsecond PRECSION */
int64_t getElapsedMicroSec(TimeStamp start, TimeStamp end);
/* get elapsed time in  nanosecond precision */
int64_t getElapsedNanoSec(TimeStamp start, TimeStamp end);


//support print with ostream
template<typename Ostream>
Ostream& operator<<(Ostream& os, TimeStamp ts)
{
    os << ts.toFmtString();
    return os;
}


// helper class to count program exec time
// class Timer
// {
// public:
//     Timer Now();
//     /* GET ELAPSED TIME IN second PRECSION */
//     static int64_t getElapsedSec(Timer start, Timer end);
//     /* GET ELAPSED TIME IN millisecond PRECSION */
//     int64_t getElapsedMilliSec();
//     /* GET ELAPSED TIME IN microsecond PRECSION */
//     int64_t getElapsedMicroSec();
//     /* get elapsed time in  nanosecond precision */
//     int64_t getElapsedNanoSec();
// private:
//     time_point<system_clock> time_;
// };

} // namespace LanceNet

#endif // LanceNet_BASE_TIME_H
