#pragma once 
#include <stdlib.h>
class MemoryAlloc;

class MemoryBlock{
public:
    int id_block;
    int nRef;
    MemoryAlloc* pMemAlloc;
    MemoryBlock* next;
};


/* singleton */

class MemoryAlloc{
public:
    MemoryAlloc(int szBlocks, int szUnit);
    MemoryAlloc() = default;
 
    void * mem_alloc(size_t nzSize);
    void mem_free(void* __ptr);
    ~MemoryAlloc();

protected:  
    /* lazy init : not init until mem_alloc is called*/
    void initMemAlloc();
    size_t m_nBlocks{0};
    size_t m_szPerBlock{0};
    MemoryBlock* m_head{nullptr};
    void* m_prawmem{nullptr};
};


template<size_t nBlocks, size_t szUnit>
class MemoryAllocator : public MemoryAlloc{
public:
    MemoryAllocator(){
        m_nBlocks = nBlocks;
        m_szPerBlock = szUnit;
    }
};


