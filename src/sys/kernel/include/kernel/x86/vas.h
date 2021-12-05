#pragma once

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

namespace kernel {
/// A class that represents a virtual address space
class vas {
   public:
    vas() {}
    vas(virt_addr_t virt, virt_addr_t phys) : directory((page_directory_raw_t*)virt), directory_addr(phys) {}

    vas*        clone();
    bool        map(phys_addr_t phys, virt_addr_t virt, uint32_t flags);
    bool        unmap(virt_addr_t addr);
    phys_addr_t physical_addr() const { return directory_addr; }
    inline void tlb_flush_single(unsigned long addr) { asm volatile("invlpg (%0)" ::"r"(addr) : "memory"); }
    void set_table_physical(int index, uintptr_t address) { pt_virt[index] = address; }

   private:
    page_directory_raw_t* directory;
    phys_addr_t           directory_addr;
    virt_addr_t           pt_virt[1024];
};
}  // namespace kernel
