#include "stdio.h"
#include "object_pool_base.hpp"
#include "alloc.h"
#include <vector>
#include <functional>
#include <thread>
#include <pthread.h>
using namespace std; 

class A : public ObjectPoolBase<A, 1>{
public:
    A(int a){
        _a = a;
        // printf("A()\n");
    }
    ~A(){
        // printf("~A()\n");
    }
private:
    int _a;
};

class B : public ObjectPoolBase<B, 100>{
public:
    B(int a, int b){
        _a = a;
        _b = b;
        // printf("B()\n");
    }
    ~B(){
        // printf("~B()\n");
    }
private:
    int _a;
    int _b; 
};

void* work(void *){
    /* do some object allocate stuff */
    /* use memery pool if alloc.h is included and linked*/
    B* p23 = B::crateObject(1,5);
    B::destoryObject(p23);
    B* p24 = new B(2,3);
    delete p24;

    B* p25 = new B(2,3);
    B* p26 = new B(2,3);
    B* p27 = new B(2,3);
    delete p25;
    delete p26;
    delete p27;
    return NULL;
}

/* WARN : std::thread can suck if use my memory memoery pool, sounds like some bug in std::thread but i can`t figure it out */
int main(){ 
    const int N = 10;
    pthread_t tids[N]; 
    for(int i = 0; i < N; i++){
        pthread_create(&tids[i], NULL, work, NULL);
    }

    for(int i = 0; i < N; i++){
        pthread_join(tids[i], NULL);
    }
    

    return 0;
}
