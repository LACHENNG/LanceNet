#include "timer.h"
#include <unistd.h>
#include <iostream>

using namespace std; 

int main()
{
    Timer timer;
    

    timer.update();
    sleep(2);
    cout << timer.getElapsedSecond() << "secs\n";
    cout << timer.getElapsedMilliSec() << "milliseconds" << endl;
    cout << timer.getElapsedMicroSec() << "microseconds" << endl;
    cout << timer.getElapsedNanoSec() << "nanoseconds" << endl;
        
    return 0;
}