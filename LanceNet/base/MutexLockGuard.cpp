#include "Mutex.h"
using namespace LanceNet;

MutexLockGuard::MutexLockGuard(MutexLock& mutex)
  : _mutex(mutex)
{
    _mutex.lock();
}

MutexLockGuard::~MutexLockGuard(){
    _mutex.unlock();
}
