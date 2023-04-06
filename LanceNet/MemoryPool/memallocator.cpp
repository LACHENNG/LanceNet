#include "memallocator.h"
#include <assert.h>

MemoryAlloc::~MemoryAlloc(){
    free(m_prawmem);
}

void * MemoryAlloc::mem_alloc(size_t nzSize){
    // TODO: optimize
    // std::lock_guard<std::mutex> lg(_mutex);

    if(m_prawmem == nullptr){
        initMemAlloc();
    }
    void * pMem = nullptr;
    /* no available space in pool or too big*/
    if(m_head == nullptr || nzSize > m_szPerBlock){
        MemoryBlock* block = (MemoryBlock*)malloc(sizeof(MemoryBlock) + nzSize);
        block->id_block = -1;
        block->next = nullptr;
        block->pMemAlloc = nullptr;
        block->nRef = 1;
        pMem = ((char*)block + sizeof(MemoryBlock));
    }else{

        MemoryBlock* block = m_head;
        m_head = m_head->next;
        assert(block->nRef == 0);
        block->nRef += 1;
        pMem = ((char*)block + sizeof(MemoryBlock));
    }
    return pMem; 
}

void MemoryAlloc::mem_free(void* __ptr){

    MemoryBlock* block = (MemoryBlock*)((char*)__ptr - sizeof(MemoryBlock));
    /* not managed by MemoryAlloc, free directly */
    if(block->pMemAlloc == nullptr){
        free(block);
    }
    /* return memory to pool*/
    else{
            // TODO: optimize
        // std::lock_guard<std::mutex> lg(_mutex);
        assert(block->nRef == 1);
        block->nRef = 0;
        block->next = m_head;
        m_head = block;
    }   

}

void MemoryAlloc::initMemAlloc(){
    
    m_prawmem = malloc( m_nBlocks * (sizeof(MemoryBlock) + m_szPerBlock));
    m_head = (MemoryBlock*) m_prawmem;
    /* construct memoryblock*/
    size_t offset = sizeof(MemoryBlock);
    MemoryBlock* pre = nullptr;
    for(int i = m_nBlocks - 1; i >= 0; i--){
        MemoryBlock* block = (MemoryBlock*)((char*)m_prawmem + (offset + m_szPerBlock) * i);
        block->id_block = i;
        block->next = pre;
        block->pMemAlloc = this;
        pre = block;
    }
}