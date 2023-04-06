#include "Mutex.h"
using namespace LanceNet;

MutexLock::MutexLock()
{
    pthread_mutex_init(&_mutex, NULL);
}

MutexLock::~MutexLock(){
    pthread_mutex_destroy(&_mutex);
}

void MutexLock::lock(){
    MUTEX_BEFORE_LOCK_INFO(_mutex);
    pthread_mutex_lock(&_mutex);
    MUTEX_AFTER_LOCK_INFO(_mutex);
}

void MutexLock::unlock(){
    MUTEX_BEFORE_UNLOCK_INFO(_mutex);
    pthread_mutex_unlock(&_mutex);
    MUTEX_AFTER_UNLOCK_INFO(_mutex);
}

pthread_mutex_t* MutexLock::getPthreadMutex(){
    return &_mutex;
}
