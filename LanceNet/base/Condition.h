// Author: Lance(Lang Chen) @ nwpu
//
// Brief: A simple wrapper to Pthread condition variable
#ifndef BASE_CONDITION_H
#define BASE_CONDITION_H

#include "Mutex.h"

namespace LanceNet
{

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

} // namespace LanceNet

#endif //BASE_CONDITION_H
