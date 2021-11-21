#pragma once

#include <kernel/types.h>

#define KHEAP_PAGEALIGN 0x01
#define KHEAP_PHYSADDR  0x02

class kheap {
   public:
    virt_addr_t alloc(size_t sz, int flags, phys_addr_t* paddr = nullptr);
    void        free(virt_addr_t addr);
    void        init(bool full, uintptr_t placement_addr = 0);

   private:
    bool        full            = false;
    virt_addr_t early_placement = 0xC1000000;
};

extern kheap g_heap;

void* operator new(size_t size);
void* operator new[](size_t size);
void  operator delete(void* p);
void  operator delete[](void* p);