#ifndef BASE_COUNT_DOWN_LATCH_H
#define BASE_COUNT_DOWN_LATCH_H
#include <atomic>
#include <functional>
#include "Condition.h"
#include "Mutex.h"

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
#endif
