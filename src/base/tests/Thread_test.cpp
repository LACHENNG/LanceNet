
#define DEBUG
#include "../Thread.h"
#include <functional>
#include <iostream>
#include <unistd.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

using namespace std;

void threadFunc(){
    printf("Thread Name: %s, tid: %d in threadFunc\n", ThisThread::GetCurrentThreadName(), ThisThread::Gettid());
    // more call to test if ThisThread::Gettid()  caching really works
    ThisThread::Gettid();
    ThisThread::Gettid();
    ThisThread::Gettid();
}

void threadFunc2(int val){
    printf("Thread Name: %s, tid: %d in threadFunc2\n", ThisThread::GetCurrentThreadName(), ThisThread::Gettid());
    ThisThread::Gettid();
    ThisThread::Gettid();
}

void threadFunc3(){
    printf("Thread Name: %s, tid: %d in threadFunc3\n", ThisThread::GetCurrentThreadName(), ThisThread::Gettid());
    sleep(3);
    ThisThread::Gettid();
}

class Foo{
public:
    Foo(int x)
      : _x(x)
    {
    }
    int getValue(){
        return _x;
    }
    void FooFunc(){
        printf("Thread Name: %s, tid: %d in FooFunc with Foo::_x%d\n", ThisThread::GetCurrentThreadName(), ThisThread::Gettid(), _x);
    }

    void FooFunc1(){
        printf("Thread Name: %s, tid: %d in FooFunc2 with Foo::_x%d\n", ThisThread::GetCurrentThreadName(), ThisThread::Gettid(), _x);
        sleep(4);
    }

private:
    int _x;
};

int main(){
    ThisThread::Gettid();
    ThisThread::Gettid();
    ThisThread::Gettid();

    Thread thread1(threadFunc);
    thread1.start();
    printf("thread1 tid : %d\n", thread1.tid());
    thread1.join();

    Thread thread2(std::bind(threadFunc2, 66));
    thread2.start();
    printf("thread2 tid : %d\n", thread2.tid());
    thread2.join();

    {
         Thread thread3(threadFunc3);
         thread3.start();
         printf("thread3 tid : %d\n", thread3.tid());
    }

    Foo foo(8);
    Thread thread4(std::bind(&Foo::FooFunc, &foo));
    thread4.start();
    printf("thread4 tid: %d in FooFunc, Foo::_x = %d\n", thread4.tid(), foo.getValue());

    Foo foo2(82);
    Thread thread5(std::bind(&Foo::FooFunc1, &foo2));
    printf("thread5 tid: %d in FooFunc1, Foo::_x = %d ##before start## \n", thread5.tid(), foo2.getValue());
    thread5.start();
    printf("thread5 tid: %d in FooFunc1, Foo::_x = %d ## after start## \n", thread5.tid(), foo2.getValue());

   // thread4.join();
   // thread5.join();
    printf("gettid syscalled %d times\n", ThisThread::GetCountofSysCallOfgettid());
    printf("Main returned\n");

    // each time the a Thread is careted , gettid syscall is called, so have the following invariant, +1 mean a manual call at main thread which not created by our Thread class
    assert(Thread::getCreateNumThread() + 1 == ThisThread::GetCountofSysCallOfgettid());

    pthread_exit(0);
    // return 0; // terminats the whole process
}
