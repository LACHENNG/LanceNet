/* Thread Pool (Template class))*/
/* The easiest way to avoid link errors involving templates is to declare and define them in a single xxx.h file */

#pragma once 
#include <iostream>
#include <vector> 
#include <thread> 
#include <unistd.h> 
#include <memory>                     // shared_ptr
#include <chrono> 
#include <atomic>
#include <string>
using namespace std::chrono_literals; // c++ 14

#include "sbuf.h"

template<typename T> 
class ThreadPool{
public:
    typedef T TaskType;
    ThreadPool(size_t m_n_threads = 8, size_t maximum_wait_requests = 10000);
    virtual ~ThreadPool() = default;
    
    /* Add a task waiting to be served by thread in the thread pool */
    void add_task(TaskType task);

    /* `this` ptr is passed in, the `OnRun` func is actually called*/
    static void* work(void* arg);
    virtual void OnRun(TaskType task);

    /* query state */
    bool isRuning();
private:
    /* activate threads pool (begin woking)*/
    void lunch();

    /* number of working threads in thread pool*/
    size_t m_n_threads;
    /* maximum number of request in the buf queue waiting to be served*/
    size_t m_max_wait_requests;
    /* threads array of size m_n_threads*/
    std::vector<std::thread> m_threads;
    std::atomic_bool m_running {false};
    
protected:
    /* bounded buffer with MT-safe attribute*/
    SBuf<TaskType> m_sbuf;
};


// /* template specialization*/
// template<>
// class ThreadPool<int> {
// public:
//     using TaskType = int;
//     ThreadPool(size_t m_n_threads = 8, size_t maximum_wait_requests = 10000);
//     virtual ~ThreadPool() = default;
//     void add_task(TaskType task);
//     static void* work(void* arg);
//     virtual void OnRun(TaskType task) = 0;
//     bool isRuning();
// private:
//     void lunch();
//     size_t m_n_threads;
//     size_t m_max_wait_requests;
//     std::vector<std::thread> m_threads;
//     std::atomic_bool m_running {false};
// protected:
//     SBuf<TaskType> m_sbuf;
// };



template<typename T> 
ThreadPool<T>::ThreadPool(size_t pool_size, size_t maximum_wait_requests): m_sbuf(maximum_wait_requests){
    m_n_threads = pool_size; 
    m_max_wait_requests = maximum_wait_requests;
    lunch();
}

template<typename T>
void ThreadPool<T>::lunch(){   
    m_running = true; 
    for(int i = 0; i < m_n_threads; i++){
            /* gen a thread */
            m_threads.push_back(std::thread(work, this));
            /* detach it */
            m_threads.back().detach();
    }
}

template<typename T> 
void ThreadPool<T>::add_task(TaskType task){
    m_sbuf.insert(task);
}

template<typename T> 
void* ThreadPool<T>::work(void* arg){
    ThreadPool* p = (ThreadPool*)arg;
    auto task = p->m_sbuf.remove();
    p->OnRun(task); 
}


template<typename T> 
void ThreadPool<T>::OnRun(TaskType task){
    printf("OnRun in Base ThreadPool in file threadpool.h called \n"); 
}


    
// template<typename T> 
// void ThreadPool<T>::OnRun(TaskType task){
//     // TODO: 解决粘包、拆包和串话问题，因为消息队列里面存的只是一个简单的fd, 在本次fd == k时可能是和用户A对话
//     // 然后A退出，B恰好复用的是同一个fd，加入消息队列，此时worker将取出这个和上次一样的fd，OnRun如何区分是用户A还是用户B
//     // Solution：不能用简单的fd作为任务类型，此时fd应该是一个携带 fd, szbuf, msgbuf, 等包括缓冲区等多个信息，这样对于新用户，
//     //       szbuf, msgbuf 初始为空，worker容易区分，还可以加入ip ，port等信息，进一步供worker鉴别当前是和谁对话，防止串话
    
    
// }


template<typename T> 
bool ThreadPool<T>::isRuning(){
    return m_running;
}


