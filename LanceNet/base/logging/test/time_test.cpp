#include "Time.h"
#include "time.h"
#include <unistd.h>
#include <iostream>
using namespace std;
using namespace LanceNet;

int main()
{
    auto start = TimeStamp::now();
    sleep(1);
    auto end = TimeStamp::now();
    cout << "Elapsed time : " << getElapsedMicroSec(start, end) << " mirco sec " << endl;
    cout << "Elapsed time : " << getElapsedNanoSec(start, end) << " nano sec " << endl;
    cout << "Elapsed time : " << getElapsedMilliSec(start, end) << " million sec " << endl;
    cout << "Elapsed time : " << getElapsedSec(start, end) << " sec " << endl;

    cout << "Timestamp: " << start.toString() << endl;
    cout << "Timestamp Fmt: " << start.toFmtString() << endl;
    cout << "Timestamp start < end ?  " << (start < end ? "Yes" : "No") << endl;
    cout << "Timestamp start == end ? " << (start == end ? "Yes" : "No") << endl;

    auto illegalTs = TimeStamp::illegal();
    cout << "IllegalTs is illegal ?" << (!illegalTs.isValid() ? "Yes" : "No") << endl;

    TimeStamp now(system_clock::now());
    cout << now.toFmtString() << endl;
    cout << now.toString() << endl;

    cout << now << endl;
    return 0;
}
