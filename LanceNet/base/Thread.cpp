// Author: Lang Chen @ nwpu

#include "Thread.h"
using namespace LanceNet;

std::atomic_uint32_t Thread::createdThreads(0);

Thread::Thread(ThreadFunc threadFunc, std::string name)
  : _joined(false),
    _started(false),
    _cached_tid(ThisThread::INVALID_THREAD_ID),
    _thread_name(name),
    _threadFunc(std::move(threadFunc)),
    _countdownlatch(1)
{
}

Thread::~Thread()
{
    if(_started && !_joined){
        detach();
    }

}
//
void Thread::start(){
    assert(!_started);
    _started = false;
    if(pthread_create(&_pthreadId, NULL, &Thread::startThread, this)){
        abort();
    }else{
        _countdownlatch.wait(); // wait for some internal state to be initialized(e.g. tid) and launched
        createdThreads += 1;
        _started = true;        // indicate the _cached_tid and some other vars that set by startThread is really
    }
}

void Thread::join(){
    assert(_started && !_joined);
    if(isJoinable())
    {
        pthread_join(_pthreadId, NULL);
        _joined = true;
    }
}

void Thread::detach(){
    if(_started && !_joined)
    {
        pthread_detach(_pthreadId);
    }
}

bool Thread::isJoinable(){
    return !_joined;
}

// get a unique system-wide thread id of current thread (return INVALID_THREAD_ID when thread is not started)
// or directly call ThisThread::Gettid() could do the same trick
pid_t Thread::tid(){
    pid_t tid = ThisThread::INVALID_THREAD_ID;
    if(_started){
        tid = _cached_tid;
    }
    return tid;
}

void* Thread::startThread(void * obj){
    Thread* threadObj = static_cast<Thread*>(obj);
    // make a explict call to cache thread id
    threadObj->_cached_tid = ThisThread::Gettid();
    ThisThread::SetCurrentThreadName(threadObj->_thread_name);

    assert(std::string(ThisThread::GetCurrentThreadName())
            == threadObj->_thread_name);

    // init finish, count down latch
    threadObj->_countdownlatch.countdown();
    threadObj->_threadFunc();
    return NULL;
}

unsigned int Thread::getCreateNumThread()
{
    return Thread::createdThreads.load();
}
