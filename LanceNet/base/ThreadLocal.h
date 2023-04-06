// Author: Lang Chen @ nwpu
// 
// Brief: create a T object with thread local storage lifetime with no arguments
// the data of Type T which automatically allocated internally by ThreadLocal is guaranteed to be released when thread exits
#ifndef Lance_BASE_THREAD_LOCAL_H
#define Lance_BASE_THREAD_LOCAL_H

#include <pthread.h>
#include "noncopyable.h"
#include <stdio.h>

namespace LanceNet
{

template<typename T>
class ThreadLocal : noncopyable{
public:
    ThreadLocal(){
        pthread_key_create(&_key, &ThreadLocal::destructor);
    }

    ~ThreadLocal(){
        pthread_key_delete(_key);
    }

    // lazy initialize
    T& getData(){
        T* _perThreadLocalData = static_cast<T*>(pthread_getspecific(_key));
        if(_perThreadLocalData == NULL){
            T* newObj = new T();
            pthread_setspecific(_key, newObj);
            _perThreadLocalData = newObj;
        }
        return *_perThreadLocalData;
    }

private:
    // used for pthread_key_create to free allocated data when thread exits.
    static void destructor(void * _data){
        delete static_cast<T*>(_data);
    }
private:
    pthread_key_t _key;
};

} // namespace LanceNet

#endif // LanceNet_BASE_THREAD_LOCAL_H
