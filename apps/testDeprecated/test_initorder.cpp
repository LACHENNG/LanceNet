#include <iostream>
using namespace std; 

struct A{
    A(){
        cout << "construct A" << endl;
    }
}; 
struct B{
    B(int * pdata){
        cout << "construct B data: "<< *pdata << endl;
    }
}; 
struct C{
    C(){
        cout << "construct C"<< endl; 
    }
};

struct T{
    T() :b(&this->num){
        printf("construct T: a = %d\n", num);
    }
public:
    
    A a;
    
    B b;
    int num{-1};
    C c;
};

int main(){
    T t;
    auto p = &(t.num);
    getchar(); 
    return 0;
}