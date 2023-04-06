// Author: Lance(Lang Chen) @ nwpu
//
// Brief: A RAII-style pthread mutex Lock wrapper
//        alone with some simple tools for debugging lock-related errors
#ifndef BASE_MUTEX_H
#define BASE_MUTEX_H
#include "noncopyable.h"
#include <pthread.h>

#ifdef DEBUG 
    #include <stdio.h>
    #include <time.h>

#define MUTEX_BEFORE_LOCK_INFO(mutex) do{ \
    printf("Trying to lock mutex %p timestamp %ld \n", &mutex, time(NULL)); \
}while(0);

#define MUTEX_AFTER_LOCK_INFO(mutex) do{ \
    printf("Locked mutex %p timestamp %ld \n", &mutex, time(NULL)); \
}while(0);

#define MUTEX_BEFORE_UNLOCK_INFO(mutex) do{ \
    printf("Trying to unlock mutex %p timestamp %ld \n", &mutex, time(NULL)); \
}while(0);

#define MUTEX_AFTER_UNLOCK_INFO(mutex) do{ \
    printf("Unlocked mutex %p timestamp %ld \n", &mutex, time(NULL)); \
}while(0);

#else

#define MUTEX_BEFORE_LOCK_INFO(mutex)

#define MUTEX_AFTER_LOCK_INFO(mutex)

#define MUTEX_BEFORE_UNLOCK_INFO(mutex)

#define MUTEX_AFTER_UNLOCK_INFO(mutex)

#endif // DEBUG


namespace LanceNet
{

class MutexLock : noncopyable{
public:
    MutexLock();
    ~MutexLock();

    /* better not used directly by user code, use RAII-style MutexLockGuard to handle lock && unlock */
    void lock();
    /* better not used directly by user code, use RAII-style MutexLockGuard to handle lock && unlock */
    void unlock();

    // only used by Condition
    pthread_mutex_t* getPthreadMutex();

private:
    pthread_mutex_t _mutex;
};



class MutexLockGuard : noncopyable{
public:
    explicit MutexLockGuard(MutexLock& mutex);
    ~MutexLockGuard();

private:
    MutexLock& _mutex;
};

// #define MutexLockGuard(x) static_assert(false, "guard object name needed" ) 
} // namespace LanceNet

#endif //!BASE_MUTEX_H
