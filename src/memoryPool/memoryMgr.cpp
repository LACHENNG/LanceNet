#include "memoryMgr.h"
#include "assert.h"

#include <stdio.h>

void MemoryMgr::initszAlloc(){
    //TODO: generalize
    for(int i = 0; i <= 64; i++) _szAlloc[i] = &_mem64;
    for(int i = 65; i <= 128; i++) _szAlloc[i] = &_mem128;
    for(int i = 129; i <= 256; i++) _szAlloc[i] = &_mem256;
    for(int i = 257; i <= 512; i++) _szAlloc[i] = &_mem512;
    for(int i = 513; i <= 1024; i++) _szAlloc[i] = &_mem1024;
}

MemoryMgr::MemoryMgr(){
    initszAlloc();
}

void * MemoryMgr::mem_malloc(size_t szSize){
    assert(szSize != 0);
    MemoryAlloc* alloc = _szAlloc[szSize % (ALLOC_SIZE_RAGNG + 1)];
    assert(alloc != nullptr);
    auto rv = alloc->mem_alloc(szSize);
#ifdef DEBUG
    MemoryBlock* block =  (MemoryBlock*)((char*)rv - sizeof(MemoryBlock));
    XPrint("Alloc: Address: %p Id: %d It`s Alloc: %p\n", rv, block->id_block, block->pMemAlloc);
#endif
    return rv;    
}

void MemoryMgr::mem_free(void *__ptr){
    size_t offset = sizeof(MemoryBlock);
    MemoryAlloc* allocator = (MemoryAlloc*)((char*)__ptr - offset);
#ifdef DEBUG
    MemoryBlock* block =  (MemoryBlock*)((char*)__ptr - sizeof(MemoryBlock));
    XPrint("Free: Address: %p Id: %d It`s Alloc: %p\n", __ptr, block->id_block, block->pMemAlloc);
#endif
    allocator->mem_free(__ptr);
}

MemoryMgr& MemoryMgr::getInstance(){
    static MemoryMgr _instance;
    return _instance;
}


