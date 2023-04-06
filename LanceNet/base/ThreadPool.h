// Author: Lance(Lang Chen) @ nwpu
#ifndef LanceNet_BASE_THREAD_POOL_H
#define LanceNet_BASE_THREAD_POOL_H

#include "noncopyable.h"
#include "BlockingQueue.h"
#include "Thread.h"
#include <atomic>
#include <cassert>
#include <vector>

#include <functional>

namespace LanceNet
{
// thread pool for executing some computing tasks, so the Task is more like any executable , may std::function, lambdas or other raw c functions
// Design brief: 
//     Traditional: make Thread::
//     My: I use std::bind and std::function to replace virtuals, following the Suggestions of Scott Meyers [EC3, item 35]
class ThreadPool : noncopyable
{
public:
    using Task = std::function<void()>;
private:
    int numThreads_;
    std::atomic_bool running_;
    std::atomic_bool started_;
    BlockingQueue<Task> tasks_;
    // Thread has object semantic rather than value sematic, reference or pointer to Thread is required 
    std::vector<Thread*> threadList_;

public:
    explicit ThreadPool(int numThreads)
      : numThreads_(numThreads),
        running_(true),
        started_(false)
    {
        assert(numThreads > 0);
    }

    void start()
    {
        // use two stage construction, it not safe to move the code into ctor in a multi-threaded condition
        for(int i = 0; i < numThreads_; i++)
        {
            threadList_.emplace_back(new Thread(std::bind(&ThreadPool::run, this)));
            threadList_.back()->start();
            threadList_.back()->detach();
//            threadList_.push_back(std::move(th));
        }
        started_ = true;
    }

    // task is a type of Universal reference
    inline void doTask(Task task)
    {
        assert(started_);
        tasks_.put(task);
    }

    void stop()
    {
        assert(started_);
        running_ = false;
    }

private:
    void run()
    {
        while(running_)
        {
            Task task = tasks_.take();
            if(task)
            {
                task();
            }
        }
    }
};

} // namespace LanceNet

#endif  // Lance_BASE_THREAD_POOL_H
