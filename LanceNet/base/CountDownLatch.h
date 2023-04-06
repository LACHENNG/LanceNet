// Author: Lance(Lang Chen) @ nwpu
//
// Brief: A simple wrapper to 
#ifndef LanceNet_BASE_COUNT_DOWN_LATCH_H
#define LanceNet_BASE_COUNT_DOWN_LATCH_H

#include <atomic>
#include <functional>
#include "Condition.h"
#include "Mutex.h"
namespace LanceNet
{

class CountDownLatch : noncopyable{
public:
    explicit CountDownLatch(int _counts);

    void countdown();

    void wait();

private:
    int _countdown;
    MutexLock _mutex;
    ConditionVariable _cv;
};

} // namespace LanceNet

#endif // LanceNet_BASE_COUNT_DOWN_LATCH_H
