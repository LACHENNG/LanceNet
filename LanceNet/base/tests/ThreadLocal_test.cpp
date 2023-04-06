#include "../ThreadLocal.h"
#include <string>
#include "../Thread.h"
using namespace std; 

// tid: 6957, Foo name: default name in Foo::ctor
// thread local1 at: 0x7f04d8000b20, name: default name
// tid: 6957, Foo name: default name in Foo::ctor
// thread local2 at: 0x7f04d8000f60, name: default name
// thread local1 at: 0x7f04d8000b20, name: John
// thread local2 at: 0x7f04d8000f60, name: Bob
// tid: 6957, Foo name: John in Foo::dtor
// tid: 6957, Foo name: Bob in Foo::dtor

class Foo
{
public:
    Foo(){
        printf("tid: %d, Foo name: %s in Foo::ctor\n", Thread::getCallingThreadId(), _str.c_str());
    }
    ~Foo(){
        printf("tid: %d, Foo name: %s in Foo::dtor\n", Thread::getCallingThreadId(), _str.c_str());
    }
    string name(){
        return _str;
    }

    void setname(const string& str){
        _str = str;
    }
private: 
    string _str{"default name"};
};

ThreadLocal<Foo> local1;
ThreadLocal<Foo> local2;

void printInfo(){
    printf("thread local1 at: %p, name: %s\n", &local1.getData(), local1.getData().name().c_str());
    printf("thread local2 at: %p, name: %s\n", &local2.getData(), local2.getData().name().c_str());
}

void thread_func(){
    printInfo();
    printf("______________________\n");
    local1.getData().setname("John");
    local2.getData().setname("Bob");
    printf("______________________\n");
    printInfo();
}

int main(){
    {
    Thread thread1(thread_func);
    thread1.start();
    thread1.join();
    } // the thread data of type T(Foo in this case) created in thread1 is automatically deleted

    printf("----------------------------------Main Below-----------------------------------\n");
    // a new call to ThreadLocal.getData() would create a defualt Foo in current thread(Main Thread) again
    printInfo();
    sleep(3);
    printf("Main Exit\n");
    pthread_exit(0);
    return 0;
}


