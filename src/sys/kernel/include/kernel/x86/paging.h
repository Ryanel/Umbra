#pragma once

#include <kernel/log.h>
#include <stdint.h>

typedef struct paging_page {
    union {
        struct {
            unsigned int present : 1;
            unsigned int write : 1;
            unsigned int user : 1;
            unsigned int write_through : 1;
            unsigned int cache_disable : 1;
            unsigned int accessed : 1;
            unsigned int dirty : 1;
            unsigned int page_attribute_table : 1;
            unsigned int available : 3;
            unsigned int address : 20;
        };
        uint32_t raw;
    };
} page_t;

typedef struct directory_entry {
    union {
        struct {
            unsigned int present : 1;
            unsigned int write : 1;
            unsigned int user : 1;
            unsigned int write_through : 1;
            unsigned int cache_disable : 1;
            unsigned int accessed : 1;
            unsigned int avalable_2 : 1;
            unsigned int page_size : 1;
            unsigned int available : 4;
            unsigned int address : 20;
        };
        uint32_t raw;
    };
} page_directory_entry_t;

typedef struct page_table {
    page_t entries[1024];
} page_table_t;

typedef struct page_directory {
    page_directory_entry_t entries[1024];
} page_directory_t;

class paging_node {
   public:
    page_directory_t* directory;

   public:
    paging_node(page_directory_t* dir) { directory = dir; }
    void describe(uint32_t virt);
    static inline void flush_tlb_single(unsigned long addr) { asm volatile("invlpg (%0)" ::"r"(addr) : "memory"); }
    static uint32_t current_get(uint32_t virt);
    static void current_map(uint32_t phys, uint32_t virt, uint32_t flags);
    static void current_unmap(uint32_t virt);
};
