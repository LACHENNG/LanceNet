// Simulate a highly concurrent Tcp connection and send data, testing OnMessage callbacks
// The closing of the connections is also implicitly tested
#include <atomic>
#include <cstdio>
#include <iostream>
#include "LanceNet/base/Thread.h"
#include "LanceNet/base/unix_wrappers.h"
using namespace std;
using Thread = LanceNet::base::Thread;

atomic_int32_t counter{0};

void threadFunc()
{
    int conn_fd = Open_clientfd("127.0.0.1", 2345);

    for(int i = 0; i < 100; i++){
        char buf[32];
        const char* msg = "hello";
        ++counter;
        snprintf(buf, 31, "%s counter: %d", msg, counter.load());
        Write(conn_fd, buf, strlen(msg));
    }
    Close(conn_fd);
    std::cout << "client exit\n";
}

int main()
{
    const int nthreads = 1000;
    for(int i = 0; i < nthreads; i++){
        Thread th(threadFunc);
        th.start();
        th.detach();
    }

    pthread_exit(0);
    // return 0;
}
