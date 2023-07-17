
#pragma once

#include <atomic>

using std::cout;

class EchoServer;

/* the workers get the connected fds and do the work, and provides the exit event to it`s caller*/
template<typename TaskType>
class Workers : public ThreadPool<TaskType>{
public:

    Workers(size_t n_threads, size_t max_reqs, EchoServer* caller_this);
    //  OnRun() override;
    
private:
    /* caller class pointer(no RAII-style needed as it belongs to echoServer and will be destoryed when echoServer is deconstructed) */
    EchoServer* m_caller_this; 
};

template<typename TaskType>
Workers<TaskType>::Workers(size_t n_threads, size_t max_reqs, EchoServer* caller_this) 
    : ThreadPool<TaskType>(n_threads, max_reqs, [this](){
        TaskType task = std::move(this->removeTask());
        task->doWork();
    })
    ,m_caller_this(caller_this)
{   
    
}

// template<typename TaskType>
// void Workers<TaskType>::OnRun(){
//     while(this->isRuning()){
//         TaskType task = std::move(this->removeTask());
//         task->doWork();
//     }
// }