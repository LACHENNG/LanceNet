// Author: Lance(Lang Chen) @ nwpu
//
// Brief: Providing get thread id Gettid() and GetCurrentThreadName
//        use GCC buliltin thread local to store and cache result

#ifndef BASE_THIS_THREAD_H
#define BASE_THIS_THREAD_H
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <string>
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#endif

namespace LanceNet
{

namespace ThisThread
{

const int INVALID_THREAD_ID = 0;
const int THREAD_NAME_BUF_SIZE = 32;

extern __thread pid_t tl_cached_tid;                      // cached thread id of syscall gettid, prefix `tl` is a short for thread_local
extern __thread char tl_namebuf[THREAD_NAME_BUF_SIZE];    // thread name buf, the calling thread is responsible for initializing it
extern __thread char* tl_threadName;                      // thread name ptr, the calling thread is responsible for resetting it pointing tl_namebuf when name is well-filled into namebuf
extern int ncalls_gettid;
// get system-wise thread id of current calling thread
inline pid_t Gettid()
{
    return (tl_cached_tid == INVALID_THREAD_ID ? tl_cached_tid = ::syscall(SYS_gettid) : tl_cached_tid );
}

inline char* GetCurrentThreadName()
{
    if(!tl_threadName){
        tl_threadName = tl_namebuf;
    }
    return tl_threadName;
}

inline void SetCurrentThreadName(const char* name)
{
    strncpy(tl_namebuf, name, THREAD_NAME_BUF_SIZE - 1);
}

inline void SetCurrentThreadName(const std::string& str)
{
    SetCurrentThreadName(str.c_str());
}

inline bool isMainThread()
{
    return Gettid() == ::getpid();
}

inline int GetCountofSysCallOfgettid()
{
    return ncalls_gettid;
}

} // namespace LanceNet

} // namespace: ThisThread

#endif // THIS_THREAD_H
