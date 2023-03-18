
#include "alloc.h"
#include <stdlib.h>
#include <timer.h>
#include <iostream>
using namespace std;

int main(int argc, char** argv, char* envp[]){
    
    // char** env = envp;
    // while(*env){
    //     printf(":  %s\n", *env);
    //     env++;
    // }
    // char * p1 = new char;
    // delete p1;
    const int N = 10000;
    char* ptrs[N];


    // printf("---------------------------------------\n");
    // ptrs[0] = new char[10];
    // snprintf(ptrs[0], 10, "%s", envp[2]);
 
    // delete ptrs[0];
    const int NR = 1;
    int repeat = NR;
    Timer timer;
    timer.update();
    while(repeat--){
        for(int i = 0; i < N; i++){
            int randSize = rand() % 1024 + 1;
            ptrs[i] = new char[randSize];
            snprintf(ptrs[i], randSize, "%s", envp[2]);
        }
        for(int i = 0; i < N; i++){
            delete[] ptrs[i];
        }
    }
    cout << "time 1: " << timer.getElapsedMilliSec() << "ms" << endl;

    repeat = NR;

    timer.update();
    while(repeat--){
        for(int i = 0; i < N; i++){
            int randSize = rand() % 1024 + 1;
            ptrs[i] = (char*)malloc(randSize);
            snprintf(ptrs[i], randSize, "%s", envp[2]);
        }
        for(int i = 0; i < N; i++){
            free(ptrs[i]);
        }
    }
    cout << "time 2: " << timer.getElapsedMilliSec() << "ms" << endl;
    return 0;
    // delete p2;
}