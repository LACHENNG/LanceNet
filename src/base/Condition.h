#ifndef BASE_CONDITION_H
#define BASE_CONDITION_H
#include "Mutex.h"

class ConditionVariable : noncopyable{
public:
    ConditionVariable(MutexLock& mutexlock);
    ~ConditionVariable();

    void wait();
    void notifyOne();
    void notifyAll();
private:
    MutexLock& _mutex;
    pthread_cond_t _cv;
};

#endif //!BASE_CONDITION_H
