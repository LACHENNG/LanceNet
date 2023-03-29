// Author: Lang Chen @ nwpu

#include "../BoundedBlockingQueue.h"
#include "../BlockingQueue.h"
#include "../Thread.h"
#include <initializer_list>
#include <unordered_set>
#include <stdio.h>
#include <iostream>
using namespace std;

BoundedBlockingQueue<int> que(1000);
unordered_set<int> uSet;
CountDownLatch latch(2);

const int kbasecount = 100000;
const int nthreads = 2;
const int STOP_FLAG = -1;

void writer(){
    for(int i = 0; i < kbasecount; i++){
        que.put(i);
    }
    que.put(STOP_FLAG); // -1 to tell reader to stop 
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
        int data = que.take();
        uSet.insert(data);
        if(data == STOP_FLAG && --countdown == 0) running = false;
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
    printf("th1 joined\n");
    th2.join();
    printf("th2 joined\n");
    th3.join();
    printf("th3 joined\n");

    for(int i = 0; i < kbasecount * nthreads; i++){
        assert(uSet.count(i) == 1);
    }
    printf("test passed\n");
    return 0;
}

