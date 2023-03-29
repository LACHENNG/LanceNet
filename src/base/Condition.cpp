#include "Condition.h"

ConditionVariable::ConditionVariable(MutexLock& mutex) : _mutex(mutex)
{
    pthread_cond_init(&_cv, NULL);
}

ConditionVariable::~ConditionVariable()
{
    pthread_cond_destroy(&_cv);
}

void ConditionVariable::wait()
{
    pthread_cond_wait(&_cv, _mutex.getPthreadMutex());
}

void ConditionVariable::notifyOne(){
    pthread_cond_signal(&_cv);
}

void ConditionVariable::notifyAll()
{
    pthread_cond_broadcast(&_cv);
}
