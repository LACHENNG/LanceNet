#include "timer.h"


Timer::Timer(){
    _begin = high_resolution_clock::now();
}

void Timer::update(){
    _begin = high_resolution_clock::now();
}
/* GET ELAPSED TIME IN second PRECSION */
double Timer::getElapsedSecond(){
    return getElapsedMicroSec() * 0.001 * 0.001;
}

/* GET ELAPSED TIME IN millisecond PRECSION */
LL Timer::getElapsedMilliSec(){
    return getElapsedMicroSec() * 0.001;
}
/* GET ELAPSED TIME IN microsecond PRECSION */
double Timer::getElapsedMicroSec(){
    auto _end = high_resolution_clock::now();
    return duration_cast<microseconds>(_end - _begin).count();
}
/* get elapsed time in  nanosecond precision */
double Timer::getElapsedNanoSec(){
    return getElapsedMicroSec() * 1000;
} 

