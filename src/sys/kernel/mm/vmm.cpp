#include <assert.h>
#include <kernel/log.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/tasks/critical_section.h>
#include <kernel/config.h>

using namespace kernel;
using namespace tasks;

virt_mm         kernel::g_vmm;
extern "C" void set_page_table(uint64_t ptr);

virt_addr_t* virt_mm::mmap(virt_addr_t virt, size_t length, int protection, int flags) {
    critical_section cs;  // TODO: Turn this into a lock on this specific core (unless virt > kernel vbase?)

    vas* current = current_vas();
    virt         = get_virt_addr(virt & 0xFFFFFFFFFFFFF000, flags);

    if (flags & VAS_HUGE_PAGE) {
        // TODO: Allow this in the future.
        kernel::log::warn("vmm", "Attempted to demand map huge page @ 0x%16p. This is unsupported.\n", virt);
        flags |= VMM_FLAG_POPULATE;
    }

    for (size_t i = 0; i < length; i += PAGE_SIZE) {
        phys_addr_t paddr = 0x0;

        if (flags & VMM_FLAG_POPULATE) { paddr = kernel::g_pmm.alloc_single(PMM_REGION_RAM); }
        if (current->map(0x0, virt + i, protection, flags) == false) { panic("Unable to map page"); }

        current->tlb_flush_single(virt + i);
        set_page_table(current->physical_addr());  // Force flush pagetable
    }

    return (virt_addr_t*)virt;
}

virt_addr_t* virt_mm::mmap_direct(virt_addr_t virt, phys_addr_t phys, int protection, int flags) {
    critical_section cs;

    vas* current = current_vas();
    virt         = get_virt_addr(virt & 0xFFFFFFFFFFFFF000, flags);

    flags |= VMM_FLAG_POPULATE;  // Direct MMAPs always will have storage populated

    current->map(phys, virt, protection, flags);
    set_page_table(current->physical_addr());
    g_pmm.mark_used(phys);

    return (virt_addr_t*)virt;
}

/// Unmaps a region of virtual memory from a tasks virtual memory.
void virt_mm::munmap(virt_addr_t virt, size_t length) {
    critical_section cs;

    vas* current = current_vas();
    virt         = virt & 0xFFFFFFFFFFFFF000;
    for (size_t i = 0; i < length; i += PAGE_SIZE) { current->unmap(virt + i); }
}

virt_addr_t virt_mm::get_virt_addr(virt_addr_t addr, int flags) {
    if (addr == 0) {
        if (flags & VMM_FLAG_KERNEL_REGION) {
            assert(false && "Fix get_virt_addr");
            addr = 0xD0000000;
        }
        while (current_vas()->get_page(addr) != 0) { addr += PAGE_SIZE; }
    }
    return addr;
}

void virt_mm::fulfill_demand_page(virt_addr_t virt) {
    vas*        current       = current_vas();
    virt_addr_t aligned_vaddr = virt & 0xFFFFFFFFFFFFF000;
    page_t      pg            = current->get_page(aligned_vaddr);

    assert((pg & VAS_HUGE_PAGE) == 0 && "Huge page demand mapping not implemented");

    phys_addr_t physpage = kernel::g_pmm.alloc_single(PMM_REGION_RAM);

    current->map(physpage, aligned_vaddr, pg & 0b1111111, VMM_FLAG_POPULATE);
}
