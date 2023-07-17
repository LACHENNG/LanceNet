
/* An echo server that supports high concurrency 
 * Author: Lang@nwpu
 * Date: 2023/3/11
 */

#pragma once
#include <sys/types.h>
#include <sys/socket.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <assert.h>
#include <atomic>
#include <vector>
#include <queue>
#include <unordered_set>
#include <functional>
#include <iostream>

#include "timer.h"
#include "unix_wrappers.h"
#include "threadpool.h"
// EchoServer and echoWorkers refer to each other
using namespace std;

// Forward declaration of class B
    // #include "echoWorkers.h" /* don`t include in both two header file of the cycle reference file 
template <typename TaskType> class Workers; 

class ClientSession;

bool cmp(ClientSession* a, ClientSession* b);

class EchoServer{
public:    
    EchoServer(int listen_port);
    ~EchoServer();

    void loop();
    /* start and stop server */
    void start();
    void stop();

    /* invoked by workers */
    void OnClientExit(ClientSession* client);
    void addRecvBytes(size_t add_cnt);
    /* console user interface */
    void UI();
private:

    /* fd_set is a fixed size buffer in glibc.  Executing FD_CLR() or FD_SET() with a value of fd that is  negative or is equal to  or  larger  than FD_SETSIZE will result in undefined behavior*/
    fd_set m_read;
    fd_set m_ready;

    int max_fd;
    int m_eventfd; 
    int listenfd;
    unordered_set<ClientSession*> m_active_clients;
    std::atomic_bool m_bRuning;

    /* store the max socket file descriptor (maxfds) of current active connections */
    //TODO: using ClientSession

    priority_queue<ClientSession*, vector<ClientSession*>, decltype(&cmp)> m_maxheap{cmp};

    /* UI thread that communicate with user input*/
    std::thread m_UIThread;
    
    /* working threads */
    Workers<ClientSession*>* m_pworkers;

    /* exist message queue related */
    unordered_set<ClientSession*> m_exit_msgs;
    std::mutex m_mutex_existed_fds;

    /* statistics related */
    Timer m_timer; 
    atomic_int total_closed{0}; 
    atomic_int total_conn{0}; 
    atomic_int m_totalbytes{0};  
};
