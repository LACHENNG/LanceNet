#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int _counts)
  : _countdown(_counts),
    _cv(_mutex)
{
}

void CountDownLatch::countdown(){
    MutexLockGuard lock(_mutex);
    if(_countdown > 0){
        _countdown--;
    }
    if(_countdown == 0){
        _cv.notifyOne();
    }
}

void CountDownLatch::wait(){
    MutexLockGuard lock(_mutex);

    while(_countdown > 0){
        _cv.wait();
    }
}


