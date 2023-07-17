/* demo program of using c11 atomic  type*/
/* can deal with built-in types atomically */
#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>
#include <vector> 
using namespace std::chrono_literals;
using namespace std;

atomic_int32_t sum{0};

void add(){
    for(int i = 0; i < 100000; i++)
        sum++;
}
int main(){

    int nthread = 8;
    vector<thread> v;
    for(int i = 0; i < nthread; i++){
        v.push_back(thread(add));

    }
    
    for(auto & th : v){
        th.join();
    }
    cout << sum << endl;
    return 0;
}