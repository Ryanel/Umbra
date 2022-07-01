#pragma once

#include <kernel/config.h>
#include <kernel/types.h>
#include <kernel/util/buddy_bitmap.h>

#define PMM_REGION_RAM 0x1
#define PMM_DMA        0x2

namespace kernel {
namespace mm {

class pmm {
   public:
    struct region {
        enum class region_type { rom, ram, dma, unmapped };
        phys_addr_t m_start;
        phys_addr_t m_end;
        region_type m_type;

        region() : m_start(0), m_end(0), m_type(region_type::unmapped) {}
        region(phys_addr_t start, phys_addr_t end, region_type type) : m_start(start), m_end(end), m_type(type) {}
    };

   public:
    void        init();
    phys_addr_t alloc_single(int flags);
    phys_addr_t alloc_contiguous(int flags);
    void        free_single(phys_addr_t addr);

    bool used(phys_addr_t addr) { return bitmap.used(addr); }
    void mark_used(phys_addr_t addr) { bitmap.mark(addr); }

    void add_region(region r);

    void update_statistics();
    void print_statistics();

   private:
    const static phys_addr_t maxAddr = 0xFFFFF000;
    buddy_bitmap             bitmap;
    region                   regions[20];
    size_t                   region_count      = 0;
    phys_addr_t              opt_firstFreeAddr = 0;
    uint32_t                 ram_pages_used    = 0;
    uint32_t                 ram_pages_max     = 0;
};
}  // namespace mm

extern mm::pmm g_pmm;

}  // namespace kernel