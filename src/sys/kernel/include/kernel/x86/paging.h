#pragma once

#include <stdint.h>
#include <kernel/log.h>

typedef struct paging_page {
    union {
        struct {
            unsigned int address : 20;
            unsigned int available : 3;
            unsigned int page_attribute_table : 1;
            unsigned int dirty : 1;
            unsigned int accessed : 1;
            unsigned int cache_disable : 1;
            unsigned int write_through : 1;
            unsigned int user : 1;
            unsigned int write : 1;
            unsigned int present : 1;
        };
        uint32_t raw;
    };
} page_t;

typedef struct directory_entry {
    union {
        struct {
            unsigned int address : 20;
            unsigned int available : 4;
            unsigned int page_size : 1;
            unsigned int avalable_2 : 1;
            unsigned int accessed : 1;
            unsigned int cache_disable : 1;
            unsigned int write_through : 1;
            unsigned int user : 1;
            unsigned int write : 1;
            unsigned int present : 1;
        };
        uint32_t raw;
    };

    void describe() {
        klogf("paging","0x%08x - Address: %d\n", raw, address);
    }
} page_directory_entry_t;

typedef struct page_table {
    page_t entries[1024];
} page_table_t;

typedef struct page_directory {
    page_directory_entry_t entries[1024];
} page_directory_t;

class page_dir_hnd {
   public:
    page_directory_t* directory;

    uint32_t get_phys_addr(uint32_t virt) { 
        directory->entries[0].describe();
        return directory->entries[0].raw;
    }
};
