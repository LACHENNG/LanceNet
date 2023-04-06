#include "../BlockingQueue.h"
#include "../Thread.h"
#include <initializer_list>
#include <unordered_set>
#include <stdio.h>
using namespace std;
#include <boost/circular_buffer.hpp>

BlockingQueue<int> que;
unordered_set<int> uSet;
CountDownLatch latch(2);

const int kbasecount = 100000;
const int nthreads = 2;
const int STOP_FLAG = -1;

void writer(){
    for(int i = 0; i < kbasecount; i++){
        que.put(i);
    }
    que.put(STOP_FLAG);
}

void writer2(){
    for(int i = kbasecount; i < 2 * kbasecount; i++){
        que.put(i);
    }
    que.put(STOP_FLAG);
}

void reader(){
    bool running = true;
    int countdown = nthreads;
    while(running){
        int data = que.get();
        uSet.insert(data);
        if(data == STOP_FLAG && --countdown == 0)
            running = false;
    }
}

int main(){
    // two writers
    Thread th1(writer);
    Thread th2(writer2);
    // one reader
    Thread th3(reader);

    th1.start();
    th2.start();
    th3.start();

    th1.join();
    th2.join();
    th3.join();

    for(int i = 0; i < 200000; i++){
        assert(uSet.count(i) == 1);
    }
    printf("test passed\n");
    return 0;
}

