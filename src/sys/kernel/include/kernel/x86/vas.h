#pragma once

#include <kernel/types.h>

#define VAS_PRESENT       0x1
#define VAS_WRITABLE      0x2
#define VAS_USER          0x4
#define VAS_HUGE_PAGE     (1 << 7)
#define VAS_DEMAND_MAPPED (1 << 9)

#ifdef ARCH_X86

#error Unimplemented

#else
// 64-bit
#define NUM_PAGES_IN_DIR 512
typedef uint64_t page_t;
typedef uint64_t vas_entry_t;
#endif

typedef struct page_table {
    page_t entries[NUM_PAGES_IN_DIR];
} pml1_t;

typedef struct page_directory_raw {
    vas_entry_t entries[NUM_PAGES_IN_DIR];
} pml2_t;

typedef page_directory_raw pml_t;

namespace kernel {
/// A class that represents a virtual address space
class vas {
   public:
    vas(virt_addr_t virt, phys_addr_t phys) : directory((pml_t*)virt), directory_addr(phys) {}
    vas*        clone();
    bool        map(phys_addr_t phys, virt_addr_t virt, uint32_t prot, int flags);
    bool        unmap(virt_addr_t addr);
    phys_addr_t physical_addr() const { return directory_addr; }
    inline void tlb_flush_single(unsigned long addr) { asm volatile("invlpg (%0)" ::"r"(addr) : "memory"); }
    // void        set_table_physical(int index, uintptr_t address) { pt_virt[index] = address; }
    page_t get_page(uintptr_t address);
    bool   has_table(uintptr_t address);
    bool   create_table(uintptr_t address);

   private:
    pml_t*      directory;
    phys_addr_t directory_addr;

    struct table_info {
        bool   large_page;
        int    level;
        int    idx;
        int    next_idx;
        pml_t* ptr;
    };

    table_info get_table(virt_addr_t addr, bool create, unsigned int desiredLevel = 1);
};
}  // namespace kernel
