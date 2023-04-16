#include <iostream>
#include <LanceNet/base/logging/ColorText.h>
#include <LanceNet/base/logging/Logging.h>

using namespace std;
using namespace LanceNet;

int main()
{
    cout << ColorText("Red", ColorText::RED) << endl;
    cout << "This Line Should Have No Color"<< endl;
    cout << ColorText("Green", ColorText::GREEN) << endl;
    cout << "This Line Should Have No Color" << endl;
    cout << ColorText("Yellow", ColorText::YELLOW) << endl;
    cout << "This Line Should Have No Color" << endl;

    LOG_INFO << ColorText("Red ", ColorText::RED);
    LOG_INFO << ColorText("Green", ColorText::GREEN);
    LOG_INFO << ColorText("Yellow", ColorText::YELLOW);

    LOG_WARN << ColorText("Red", ColorText::RED);
    LOG_WARN << ColorText("Green", ColorText::GREEN);
    LOG_WARN << ColorText("Yellow", ColorText::YELLOW);

    LOG_FATAL << ColorText("Red", ColorText::RED);
    LOG_FATAL << ColorText("Green", ColorText::GREEN);
    LOG_FATAL << ColorText("Yellow", ColorText::YELLOW);

    LOG_INFOC << "Plain Text";
    LOG_WARNC  << "Plain Text";
    LOG_FATALC << "Plain Text";

    return 0;
}
