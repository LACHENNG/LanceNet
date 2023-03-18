#include "alloc.h"
#include "memmanager.h"
void* operator new(size_t szSize){
    return MemoryManager::getInstance().mem_malloc(szSize);

}
void* operator new[](size_t szSize){
    return MemoryManager::getInstance().mem_malloc(szSize);
}
void operator delete(void* __ptr) noexcept{
    MemoryManager::getInstance().mem_free(__ptr);
}
void operator delete[](void* __ptr) noexcept{
    MemoryManager::getInstance().mem_free(__ptr);
}
