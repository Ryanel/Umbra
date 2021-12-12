#pragma once

#include <kernel/config.h>
#include <kernel/types.h>
#include <kernel/util/bitmap.h>

#define PMM_REGION_RAM 0x1
#define PMM_DMA        0x2

namespace kernel {
namespace mm {

class buddy_bitmap {
   public:
    int          granularity = 0;
    size_t size() const { return (granularity + 1) * 0x1000; }
    bitmap       backing;

    void init(uintptr_t max_address) {
        size_t needed_ram = (max_address / size() / 8) + 1;
        // backing.array      = new uintptr_t[needed_ram];
        backing.size_bytes = needed_ram;
    }

    inline bool used(uintptr_t address) { return backing.test((address & 0xFFFFF000) / size()); }
    inline void mark(uintptr_t address) { backing.set((address & 0xFFFFF000) / size()); }
    inline bool mark_if_clear(uintptr_t address) { return backing.mark_if_clear((address & 0xFFFFF000) / size()); }
    inline void free(uintptr_t address) { backing.clear((address & 0xFFFFF000) / size()); }
};

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