// Author: Lang Chen @ nwpu
#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <pthread.h>

#include "noncopyable.h"
#include "CountDownLatch.h"
#include "ThisThread.h"

#include <functional>
#include <atomic>

#include <string>
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>
#include <sys/types.h> // gettid system call

namespace LanceNet{

namespace base{

class Thread : noncopyable
{
public:
    using ThreadFunc = std::function<void()>;

private:
    bool _joined;
    bool _started;
    static std::atomic_uint32_t createdThreads;
    // raw pthread_t handle
    pthread_t _pthreadId;
    // system-wise thread id (0 if thread not started)
    pid_t _cached_tid;
    std::string _thread_name;
    ThreadFunc _threadFunc;
    LanceNet::CountDownLatch _countdownlatch;
// private:
       // struct ThreadData{
       //     using ThreadFunc = std::function<void()>;
       //     ThreadFunc _threadFunc;
       //     pid_t * _cached_tid;
       //     ThreadData(ThreadFunc&& thread_func, pid_t* tid){
       //         _threadFunc = std::move(thread_func);
       //         _cached_tid = tid;
       //     }

       //     static void* startThread(void * threadData){
       //
       //     }
       // };
public:

    Thread(ThreadFunc threadFunc, std::string name = std::string("Default Name"));

    ~Thread();
    void start();

    void join();

    void detach();

    bool isJoinable();

    // get a unique system-wide thread id of current thread (return INVALID_THREAD_ID when thread is not started)
    // or directly call ThisThread::Gettid() could do the same trick
    pid_t tid();

    static unsigned int getCreateNumThread();

private:
    static void* startThread(void * obj);

};

} // namespace base
  //
} // namespace LanceNet

#endif
