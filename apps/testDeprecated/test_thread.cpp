#include <thread>
#include <iostream>
using namespace std; 


void fun(){
    printf("test");
}

// If a child thread is detached, then the program will not call std::terminate() when the main thread exits. The detached child thread will continue to run independently in the background until it finishes on its own. This means that even if the main thread exits, the child thread can still continue to run.

// Of course, you need to be aware that resources associated with the child thread may be cleaned up after the main thread exits. Therefore, if you want to access these resources after the main thread exits, you need to make sure they still exist after the main thread exits.

int main(){

    thread t(fun);
    // t.join();
    t.detach();
    for(int i = 0; i < 4; i++){
        cout << "main thread\n"; 
    }
   
    return 0;
}

