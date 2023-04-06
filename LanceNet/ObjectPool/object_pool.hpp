#pragma once 
#include <stdlib.h>
#include <assert.h>
#include <mutex>
#include <alloc.h>
/* Note: Nested class Provides a new type scope that emphasizes the master-slave relationship between two classes */
template <typename _Type, size_t _PoolSize>
class  ObjectPool{
/* nested class to manage the user class object*/
private:
    class Node{
        public:
            int nID{-1};
            int nRef{0};
            Node* next{nullptr};
            ObjectPool* pObjPool{nullptr};
    };

public:
    ObjectPool();
    ~ObjectPool();
    
    /* allocate raw memory like a `malloc` */
    void *allocObjMemory(size_t szSize);

    /* free raw memory like a `free` */
    void freeObjMemory(void* _ptr);

    /* like `new` */
    template<typename ...Args>
    _Type* createObject(Args... _args);

    /* like `delete` */
    void destoryObject(_Type* _pobj);

private:
    void initPool();
    
private:
    /* next available Object Node pointer*/
    Node* _pHeader{nullptr};  
    /* object pool buffer */
    char * _pBuf{nullptr}; 
    /* user class size （store  */
    const int _SZUSERCLASS = sizeof(_Type);

    std::mutex _mutex;
};

template <typename _Type, size_t _PoolSize>
void* ObjectPool<_Type, _PoolSize>::allocObjMemory(size_t szSize){
    std::lock_guard<std::mutex> lg(_mutex);
    /* no enough mem in pool*/
    if(nullptr == _pHeader){
        Node* pReturn = (Node*)new char[_SZUSERCLASS + sizeof(Node)];
        pReturn->next = nullptr;
        pReturn->nID = -1;
        pReturn->nRef = 1; 
        pReturn->pObjPool = nullptr;
        return ((char*)pReturn + sizeof(Node));
    }
    
    /* request can be satisfied by obj pool*/
    Node* pReturn = _pHeader;
    assert(pReturn->nRef == 0);
    pReturn->nRef = 1;
    _pHeader = _pHeader->next;

    printf("Addr: %p Id: %d \n", ((char*)pReturn + sizeof(Node)), pReturn->nID);
    return (_Type*)((char*)pReturn + sizeof(Node));
}

template <typename _Type, size_t _PoolSize>
void ObjectPool<_Type, _PoolSize>::freeObjMemory(void* _ptr){
    // TODO: optimize
    std::lock_guard<std::mutex> lg(_mutex);
    Node* node = (Node*)((char*)_ptr - sizeof(Node));
    /* not managed by obj pool, return mem to system*/
    if(node->pObjPool == nullptr){
        delete[] node;
    }
    /* return mem to obj pool */
    else{
        assert(node->nRef == 1);
        node->nRef = 0;
        node->next = _pHeader;
        _pHeader = node;
    }
    printf("~Addr: %p Id: %d \n", ((char*)node + sizeof(Node)), node->nID);
}


template <typename _Type, size_t _PoolSize>
template<typename ...Args>
_Type* ObjectPool<_Type, _PoolSize>::createObject(Args... _args){
    _Type* obj =  new _Type(_args...);
    /* do something you want */
    return obj;
}

template <typename _Type, size_t _PoolSize>
void ObjectPool<_Type, _PoolSize>::destoryObject(_Type* _pobj){
    delete _pobj;
}


template <typename _Type, size_t _PoolSize>
ObjectPool<_Type, _PoolSize>::ObjectPool(){
    initPool();
}

template <typename _Type, size_t _PoolSize>
ObjectPool<_Type, _PoolSize>::~ObjectPool(){
    if(_pBuf) delete(_pBuf);
}


template <typename _Type, size_t _PoolSize>
void ObjectPool<_Type, _PoolSize>::initPool(){
    assert(_pBuf == nullptr); 

    size_t sz = _PoolSize * (_SZUSERCLASS + sizeof(Node));
    _pBuf = new char[sz];
    _pHeader = (Node*) _pBuf;

    const int offset = (_SZUSERCLASS + sizeof(Node));
    for(int i = 0; i < _PoolSize; i++){
        Node* pnode = (Node*)((char*)_pBuf + i * offset);
        pnode->nID = i;
        pnode->nRef = 0;
        pnode->pObjPool = this;
        if(i != _PoolSize - 1) {
            pnode->next = (Node*)((char*)pnode + offset);
        }
        else pnode->next = nullptr;
    }
};




