/* Autor: lang @ nwpu 
 * Description: A memory pool Manager
*/

#pragma once 
#include "memalloc.h"

/* debug purpose only */
#ifdef DEBUG
    #include "stdio.h"
    #define XPrint(...) printf(__VA_ARGS__);
#else
    #define XPrint(...) 
#endif
/* !debug purpose only */
#define ALLOC_SIZE_RAGNG 1024

class MemoryMgr{
public:
    void *mem_malloc(size_t szSize);
    void mem_free(void *__ptr);

    static MemoryMgr& getInstance();
private:
    MemoryMgr();
    void initszAlloc();

    MemoryAllocator<10000, 64> _mem64;
    MemoryAllocator<10000, 128> _mem128;
    MemoryAllocator<10000, 256> _mem256;
    MemoryAllocator<10000, 512> _mem512;
    MemoryAllocator<10000, 1024> _mem1024;

    /* a map from alloc_size to _memxx pool object to speed up mem_malloc*/
    MemoryAlloc* _szAlloc[ALLOC_SIZE_RAGNG + 1]{nullptr};
};
