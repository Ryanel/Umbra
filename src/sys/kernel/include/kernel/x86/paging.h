#pragma once

#include <kernel/log.h>
#include <kernel/types.h>
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

typedef struct page_directory_raw {
    page_directory_entry_t entries[1024];
} page_directory_raw_t;

class page_directory {
   public:
    page_directory_raw_t* directory;
    phys_addr_t           directory_addr;
    virt_addr_t           pt_virt[1024];

    inline void tlb_flush_single(unsigned long addr) { asm volatile("invlpg (%0)" ::"r"(addr) : "memory"); }

   public:
    page_directory() { directory = nullptr; }
    page_directory(page_directory_raw_t* dir) { directory = dir; }
    page_directory(phys_addr_t dir) { directory = (page_directory_raw_t*)dir; }

    bool map(phys_addr_t phys, virt_addr_t virt, uint32_t flags);
    bool unmap(virt_addr_t addr);

    page_directory* clone();

   private:
    void allocate_page_directory(virt_addr_t virt);
};
