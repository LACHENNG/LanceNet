#ifndef BASE_BLOCKING_QUEUE_H
#define BASE_BLOCKING_QUEUE_H
#include "Mutex.h"
#include "Condition.h"
#include "noncopyable.h"
#include "assert.h"
#include <deque>

template<typename T>
class BlockingQueue : noncopyable{
private:
    MutexLock mutex_;
    ConditionVariable cv_;
    std::deque<T> que_;

public:
    BlockingQueue()
      : cv_(mutex_)
    {
    }

    void put(const T& data){
        MutexLockGuard lock(mutex_);
        que_.push_back(data);
        cv_.notifyOne();
    }

    void put(T&& data){
        MutexLockGuard lock(mutex_);
        que_.push_back(std::move(data));
        cv_.notifyOne();
    }

    T take(){
        MutexLockGuard lock(mutex_);
        while(que_.size() == 0){
            cv_.wait();
        }
        assert(que_.size() > 0);
        // Wrong! reference to que_font but que_.pop_front() later which destory to memery, make the reference invalid
        // T& data = que_.front();
        // que_.pop_front();
        //
        T front(std::move(que_.front()));
        que_.pop_front();

        return std::move(front);
        // Note: a temp object is a rvalue which is usually auto RVOed by the compiler
        // but data is left value so explicitly std::move
    }
};

#endif // BASE_BLOCKING_QUEUE_H
