
#pragma once
#include "threadpool.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unix_wrappers.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <assert.h>
#include "timer.h"
// #include <vector>
// #include <deque> 
#include <atomic>

#include <vector>
#include <queue>
#include <unordered_set>
#include <functional>
#include <iostream>

using std::cout;

class EchoServer;

/* the workers get the connected fds and do the work, and provides the exit event to it`s caller*/
template<typename TaskType>
class Workers : public ThreadPool<TaskType>{
public:

    Workers(size_t n_threads, size_t max_reqs, EchoServer* caller_this);
    void OnRun(TaskType task);
    // void OnMessage(int fd);
    // void OnExit(int fd);
    
private:
    /* runing state*/
    std::atomic_bool m_bRuning;
    /* caller class pointer(no RAII-style needed as it belongs to echoServer and will be destoryed when echoServer is deconstructed) */
    EchoServer* m_caller_this; 
};


template<typename TaskType>
Workers<TaskType>::Workers(size_t n_threads, size_t max_reqs, EchoServer* caller_this) 
    : ThreadPool<TaskType>(n_threads, max_reqs), m_caller_this(caller_this),
      m_bRuning(true)
{   

}

template<class TaskType>  
void Workers<TaskType>::OnRun(TaskType task)
{
    while(this->isRuning()){
        auto task = this->m_sbuf.remove();
        task->OnMessage();
    }
}

// template<class TaskType>
// // TODO: fix the Sticky packet and less packet in TCP network programming
// void Workers<TaskType>::OnMessage(int fd){
//     // if(eventloop->m_fds[fd] == -1) return;

//     static char Buf[1024]; 
//     int n = read(fd, Buf, 1024);
//     /* for nonblocking socket fd, n == 0 mean peer closed the connection*/
//     if(n == 0){
//         cout << "thread: " << std::this_thread::get_id() << " " << "OnMessage n == 0\n";
//         OnExit(fd);
//     }
//     else if(n == -1){
//         perror("read");
//     }else if(n > 0){
//         auto addBytesToServer = std::mem_fn(&EchoServer::addRecvBytes);
//         addBytesToServer(m_caller_this, n);
//         // fprintf(stdout, "from %d : %s\n", fd, Buf);
//         // write(fd, Buf, n); /* issue: race condition  */ 
//     }      
// }

// template<class TaskType>
// void Workers<TaskType>::OnExit(int fd){
//     auto exit = std::mem_fn(&EchoServer::OnClientExit);
//     exit(m_caller_this, fd);
// }