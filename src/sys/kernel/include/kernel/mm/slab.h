#pragma once

#include <stddef.h>
#include <stdint.h>

#define SLAB_MIN_ENTRIES 4
#define PAGE_SIZE        0x1000

uintptr_t power_ceil(uintptr_t x);

struct slab_entry {
    slab_entry* m_next;
};

struct slab {
    slab*       m_next       = nullptr;
    slab_entry* m_free_list  = nullptr;
    uintptr_t   m_start      = 0;
    uint32_t    m_size       = 0;
    uint16_t    m_pages      = 1;
    uint16_t    m_entries    = 0;
    uint16_t    m_maxEntries = 0;

    void init(uintptr_t start, uint32_t sz);
    bool alloc(uintptr_t sz, uintptr_t& out_address);

    static uint16_t get_pages(uintptr_t sz) {
        uint16_t pg = (sz * (SLAB_MIN_ENTRIES + 1)) / 0x1000;
        pg          = power_ceil(pg);
        return pg;
    }

    template <typename T>
    bool alloc(uintptr_t sz, T& obj_addr) {
        uintptr_t addr;
        bool      res = alloc(sz, addr);
        obj_addr      = (T)addr;
        return res;
    }

    bool free(uintptr_t address);

    void debug();
};

class slab_allocator {
   public:
    void  init(uintptr_t v_memstart, uintptr_t vmem_size);
    void* alloc(uint32_t size);
    void  free(void* ptr);
    void  debug();

   private:
    slab*     slab_meta_head;
    slab*     slab_last_allocated;
    uintptr_t m_heap_start;
    uintptr_t m_heap;
    uintptr_t m_heap_max;

    uintptr_t allocate_heap(uint32_t size);
    slab*     slab_alloc_meta();
};
