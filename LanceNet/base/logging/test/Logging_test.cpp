#include "../Time.h"
#include "iostream"
#include "../Logging.h"
using namespace LanceNet;

using namespace std;

int main()
{
    LOG_WARN << "Hello1" ;
    LOG_FATAL << "Hello2" ;
    LOG_INFO << "Hello3" ;
    
    return 0;
}
