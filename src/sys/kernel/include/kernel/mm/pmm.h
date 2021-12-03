#pragma once

#include <kernel/config.h>
#include <kernel/types.h>
#include <kernel/util/bitmap.h>

namespace kernel {

enum class pmm_region_type {
    unknown,
    ram,
    rom,
};

typedef struct pmm_region {
    pmm_region_type m_type;
    phys_addr_t     m_start;
    phys_addr_t     m_end;
    pmm_region() {}
    pmm_region(pmm_region_type type, phys_addr_t start, phys_addr_t end) : m_type(type), m_start(start), m_end(end) {}
} pmm_region_t;

// The physical memory manager keeps track of usage for each region.

class phys_mm {
   public:
    pmm_region_t   regions[KERNEL_PMM_MAXREGIONS];
    unsigned short region_count;
    // Stack for free pages

    bitmap backing_store;  // Backing bitmap, 1 = available, 0 = occupied

    void        describe() const;
    void        add_region(pmm_region_t region);
    void        init();
    phys_addr_t get_available_page();
    void        mark_used(phys_addr_t addr);
    void        mark_free(phys_addr_t addr);
    bool        page_available(phys_addr_t addr);
};

extern phys_mm g_pmm;

}  // namespace kernel