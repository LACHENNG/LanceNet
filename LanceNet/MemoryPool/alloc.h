/* Autor: lang @ nwpu 
 * Description: override global new and delete if you include "alloc.h" in your program
*/

#pragma once 
#include <stdlib.h>
void * operator new(size_t nSize);
void * operator new[](size_t nSize);

void  operator delete(void * p) noexcept;
void  operator delete[](void * p) noexcept; 
