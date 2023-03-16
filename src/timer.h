#pragma once 

#include <chrono> 
using namespace std::chrono;
using namespace std::chrono_literals;
using LL = long long;
class Timer{
public:

    Timer();

    void update();
    /* GET ELAPSED TIME IN sECOND PRECSION */
    double getElapsedSecond();
    /* GET ELAPSED TIME IN millisecond PRECSION */
    LL getElapsedMilliSec();
    /* GET ELAPSED TIME IN microsecond PRECSION */
    double getElapsedMicroSec();
    /* get elapsed time in  nanosecond precision */
    double getElapsedNanoSec();

private:
    time_point<high_resolution_clock> _begin;
};

