#include "LogStream.h"
#include "limits.h"
#include "iostream"
#include <string>
using namespace std;

#define LOG LogStream()
using namespace LanceNet;
// static const char * p = "1243";

// simple test for LogStream class
int main()
{
    // you may expect addr p is a constant
    // every time you run 'a.out' as it is stored at static storage area
    // but you are not due to the 'Address Space Layout Randomization, ASLR'
    static char p[] = "1243";
    bool b = true;
    cout << (LOG << b) << endl;
    b = false;
    cout << (LOG << b) << endl;


    short short_min = SHRT_MIN;
    short short_max = SHRT_MAX;
    cout << (LOG << short_min) << endl;
    cout << (LOG << short_max) << endl;

    unsigned short us_max = USHRT_MAX;
    cout << (LOG << us_max) << endl;

    int int_max = INT_MAX;
    int int_min = INT_MIN;
    cout << (LOG << int_min) << endl;
    cout << (LOG << int_max) << endl;

    float f = 1.1243;
    double d = 1.1234567;
    cout << (LOG << f) << endl;
    cout << (LOG << d) << endl;

    cout << (LOG << (void*)p << " " << p) << endl;

    std::string str("string test");
    cout << (LOG << str) << endl;
    return 0;
}
