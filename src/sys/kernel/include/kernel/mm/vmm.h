#pragma once

#include <kernel/config.h>
#include <kernel/mm/vas.h>
#include <kernel/types.h>
#include <stddef.h>
#include <kernel/cpu.h>

#define VMM_MMAP_READABLE        0x1
#define VMM_PROT_WRITE           0x2
#define VMM_PROT_USER            0x4
#define VMM_MMAP_DEMAND_INTERNAL (1 << 9)

#define VMM_FLAG_POPULATE      0x1
#define VMM_FLAG_KERNEL_REGION 0x2
#define VMM_FLAG_PAGE_TABLE    0x4

namespace kernel {

class virt_mm {
   public:
    /// Maps a potentially noncontiguous region of RAM to vaddr virt of size length bytes, with the specified protection
    /// flags
    virt_addr_t* mmap(virt_addr_t virt, size_t length, int protection, int flags);
    /// Forces a direct mapping between virt and phys
    virt_addr_t* mmap_direct(virt_addr_t virt, phys_addr_t phys, int protection, int flags);
    /// Unmaps a region of virtual memory from a tasks virtual memory.
    void        munmap(virt_addr_t virt, size_t length);
    void        fulfill_demand_page(virt_addr_t addr);
    virt_addr_t get_virt_addr(virt_addr_t addr, int flags);

   public:
    // Written for multi-cpu support down the line
    vas* current_vas() { return g_cpu_data[0].current_vas; }
};

extern virt_mm g_vmm;
}  // namespace kernel

