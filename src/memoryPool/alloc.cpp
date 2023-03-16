#include "alloc.h"
#include "memoryMgr.h"
void* operator new(size_t szSize){
    return MemoryMgr::getInstance().mem_malloc(szSize);

}
void* operator new[](size_t szSize){
    return MemoryMgr::getInstance().mem_malloc(szSize);
}
void operator delete(void* __ptr){
    MemoryMgr::getInstance().mem_free(__ptr);
}
void operator delete[](void* __ptr){
    MemoryMgr::getInstance().mem_free(__ptr);
}
