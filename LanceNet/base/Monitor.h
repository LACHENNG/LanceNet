#pragma once
#include "LanceNet/base/Condition.h"
#include "LanceNet/base/Mutex.h"
#include <mutex>
#include <condition_variable>

namespace LanceNet{
// A Monitor class, used to encapsulate a single mutex and a condition variable(cv)
// It is useful to slove simple synchronization problems like consumer & producer
// But when involves more than one condition variable and complicated state controls like multi-reader and writer problem 
// draw back to use the raw mutex + cv to achieve more flexibility
 class Monitor {
public:
// Constructor, accepts a T type object as an argument
    Monitor()
      : m_cv(m_mutex)
    {
    }

    // Auto lock and unlock using RAII
    class Lock{
    public:
        Lock(Monitor* monitor) : m_monitor(monitor) { monitor->m_mutex.lock(); }
        ~Lock(){ m_monitor->m_mutex.unlock(); }

        // Waits for a predicate to be satisfied on the condition variable
        template <class Predicate>
        void waitUntil(Predicate pred) {
          while (!pred()) {
             m_monitor->m_cv.wait();
          }
        }

        // Wakes up one thread waiting on the condition variable
        void notify_one() {
            m_monitor->m_cv.notifyOne();
        }

        // Wakes up all threads waiting on the condition variable
        void notify_all() {
            m_monitor->m_cv.notifyAll();
        }

    private:
        Monitor* m_monitor;
    };

    // return a lock that use RAII mechanism to auto lock and unlock
    Lock LockGurad(){
        return Lock(this);
    }

private:
    LanceNet::MutexLock m_mutex; // The mutex
    LanceNet::ConditionVariable m_cv; // The condition variable
};

} // namespace LanceNet
