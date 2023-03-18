/* Description：Thread Pool (Template class))
 * Author: Lang@nwpu
 * Date: 2023/2/11
 * Note: The easiest way to avoid link errors involving templates is to declare and define them in a single xxx.h file
*/

#pragma once 
#include <vector> 
#include <thread> 
#include <functional>                 // std::mem_fn


/* NOTE：It is not recommended to use namespace in.h files. You can use namespace in.cpp files, which will only affect the scope of one compilation unit*/
// using namespace std::chrono_literals; // c++ 14
#include "sbuf.h"


/* NOTE：Template classes can use virtual functions, but template functions cannot be virtual, 
   due to the compiler cannot determine the size of the continuation table*/
template<typename T> 
class ThreadPool{
public:
    typedef T TaskType;
    
    ThreadPool(size_t n_threads = 8, size_t max_waits = 1000000);
    virtual ~ThreadPool() = default;
    
    /* Add a task waiting to be served by thread in the thread pool */
    void addTask(TaskType task);

    /* `this` ptr is passed in, the `OnRun` func is actually called*/
    void OnRun();

    // /* query state */
    // bool isRuning();
    
private:
    /* activate threads pool (begin woking)*/
    void Start();

    /* number of working threads in thread pool*/
    size_t _n_threads;

    /* threads array of size _n_threads*/
    std::vector<std::thread> _threadList;
    // std::atomic_bool _running {false};
    
protected:
    /* bounded buffer with MT-safe attribute*/
    SBuf<TaskType> _buf_s;
};

template<typename T> 
ThreadPool<T>::ThreadPool(size_t n_threads, size_t max_waits): _buf_s(max_waits){
    _n_threads = n_threads; 
    Start();
}

template<typename T>
void ThreadPool<T>::Start(){   
    // _running = true; 
    for(int i = 0; i < _n_threads; i++){
            /* gen a thread */
            _threadList.push_back(std::move(std::thread(std::mem_fn(&ThreadPool<T>::OnRun), this)));

            /* detach it */
            _threadList.back().detach();
    }
}

template<typename T> 
void ThreadPool<T>::addTask(TaskType task){
    _buf_s.insert(task);
}

template<typename T> 
void ThreadPool<T>::OnRun(){
    
}

// template<typename T> 
// bool ThreadPool<T>::isRuning(){
//     return _running;
// }


