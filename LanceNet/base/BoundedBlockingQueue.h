// Author :  Lance(Lang Chen) @ npuw
//
// Brief: BoundedBlockingQueue is a MT-safe bounded queue base on production-comsumer model
// blocks if the queue is empty when 'take()' or blocks at 'put()' when the queue is full

#ifndef LanceNet_BASE_BOUNDED_BLOCKING_QUEUE_H
#define LanceNet_BASE_BOUNDED_BLOCKING_QUEUE_H

#include "Mutex.h"
#include "Condition.h"

#include <deque>
#include <assert.h>

namespace LanceNet
{

template<typename T>
class BoundedBlockingQueue : noncopyable{
private:
    MutexLock mutex_;
    ConditionVariable cv_notEmpty_;
    ConditionVariable cv_notFull_;
    std::deque<T> que_;
    size_t capacity_;

public:
    BoundedBlockingQueue(size_t capacity)
      : cv_notEmpty_(mutex_),
        cv_notFull_(mutex_),
        capacity_(capacity)
    {
    }

    void put(const T& item)
    {
        MutexLockGuard lock(mutex_);
        // Wrong, The MutexLock is non-reentrant and will deadlock immediately upon execution of the isFull()
        // while(isFull()){
        while(que_.size() == capacity_){
            cv_notFull_.wait();
        }
        que_.push_back(item);
        cv_notEmpty_.notifyOne();
    }

    void put(T&& item)
    {
        MutexLockGuard lock(mutex_);
        // Wrong, The MutexLock is non-reentrant and will deadlock immediately upon execution of the isFull()
        // while(isFull()){
        while(que_.size() == capacity_){
            cv_notFull_.wait();
        }
        assert(que_.size() < capacity_);
        que_.push_back(std::move(item));
        cv_notEmpty_.notifyOne();
    }

    T take()
    {
        MutexLockGuard lock(mutex_);
        // Wrong, The MutexLock is non-reentrant and will deadlock immediately upon execution of the isEmpty()
        // while(isEmpty()){
        while(que_.size() == 0){
            cv_notEmpty_.wait();
        }
        assert(que_.size() > 0);

        T& data = que_.front();
        que_.pop_front();
        cv_notFull_.notifyOne();
        return std::move(data);
    }

    bool isFull()
    {
        MutexLockGuard lock(mutex_);
        return que_.size() >= capacity_;
    }

    bool isEmpty()
    {
        MutexLockGuard lock(mutex_);
        return que_.size() == 0;
    }
};

} // namespace LanceNet

#endif // LanceNet_BASE_BOUNDED_BLOCKING_QUEUE_H
