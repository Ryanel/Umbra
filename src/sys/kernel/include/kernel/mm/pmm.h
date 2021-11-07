#pragma once

#include <kernel/config.h>
#include <kernel/types.h>

namespace kernel {

enum class pmm_region_type {
    unknown,
    ram,
    rom,
};

typedef struct pmm_region {
    pmm_region_type type;
    phys_addr_t     start;
    phys_addr_t     end;
} pmm_region_t;

// The physical memory manager keeps track of usage for each region.

class phys_mm {
   public:
    pmm_region_t   regions[KERNEL_PMM_MAXREGIONS];
    unsigned short region_count;
    // Stack for free pages
    // Backing bitmap

    void        add_region(pmm_region_t region);
    phys_addr_t get_free_page();
    void        describe() const;
};

extern phys_mm g_pmm;

}  // namespace kernel