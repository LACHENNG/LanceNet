/* Thread Pool (Template class))*/
/* The easiest way to avoid link errors involving templates is to declare and define them in a single xxx.h file */

#pragma once 
#include <iostream>
#include <vector> 
#include <thread> 
#include <unistd.h> 
#include <memory>                     // shared_ptr
#include <chrono> 
using namespace std::chrono_literals; // c++ 14

#include "sbuf.h"

std::mutex g_lock;

template<typename T> 
class ThreadPool{
public:
    ThreadPool(size_t m_n_threads = 8, size_t maximum_wait_requests = 10000);
    virtual ~ThreadPool() = default;
    /* activate threads pool (begin woking)*/
    void lunch();
    /* Add a task waiting to be served by thread in the thread pool */
    void add_task(T task);

    /* `this` ptr is passed in, the `run` func is actually called*/
    static void* work(void* arg);
    virtual void run();
private:

    /* number of working threads in thread pool*/
    size_t m_n_threads;
    /* maximum number of request in the buf queue waiting to be served*/
    size_t m_max_wait_requests;
    /* threads array of size m_n_threads*/
    std::vector<std::thread> m_threads;
    bool running {false};
protected:
    /* bounded buffer with MT-safe attribute*/
    std::shared_ptr<SBuf<T>> m_sbuf;
};

template<typename T> 
ThreadPool<T>::ThreadPool(size_t pool_size, size_t maximum_wait_requests): m_sbuf(new SBuf<T>(maximum_wait_requests)){
    m_n_threads = pool_size; 
    m_max_wait_requests = maximum_wait_requests;
}

template<typename T>
void ThreadPool<T>::lunch(){    
    for(int i = 0; i < m_n_threads; i++){
            /* gen a thread */
            m_threads.push_back(std::thread(work, this));
            /* detach it */
            m_threads.back().detach();
    }
}

template<typename T> 
void ThreadPool<T>::add_task(T task){
    m_sbuf->insert(task);
}

template<typename T> 
void* ThreadPool<T>::work(void* arg){
    ThreadPool* pool = (ThreadPool*)arg;
    pool->run(); 
}

template<typename T> 
void ThreadPool<T>::run(){
    while(true){
        /* Gets the task from the buffer queue */
        // TODO: impl move semantic
        auto task = m_sbuf->remove();
        {
        std::lock_guard<std::mutex> lg(g_lock);
        std::cout << task << std::endl;
        }
    }
}