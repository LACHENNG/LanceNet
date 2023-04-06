
#include <memory>
#include <assert.h>
#include "object_pool.hpp"

/* no datamembers, thus not influence the derived class */
template<class T, size_t PoolSize>
class ObjectPoolBase{
public:

    void * operator new(size_t sz){
       return getObjectPool()->allocObjMemory(sz);
    }

    void operator delete(void *_ptr){
        getObjectPool()->freeObjMemory(_ptr);
    }

    /* allocate T memory (not constructed)*/
    template<typename ...Args> 
    static T* crateObject(Args... args){
  
        T* obj = getObjectPool()->createObject(args...);
        /* do some else*/
        return obj;
 
    }

    static void destoryObject(T* obj){
        getObjectPool()->destoryObject(obj);
    }

private:
    using ObjectClassTPool = ObjectPool<T, PoolSize>;
    static ObjectClassTPool* getObjectPool(){
        static ObjectClassTPool _objpool;
        return &_objpool;
    }

};