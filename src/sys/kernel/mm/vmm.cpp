#include <kernel/log.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/tasks/critical_section.h>

using namespace kernel;
using namespace tasks;

virt_mm kernel::g_vmm;

virt_addr_t* virt_mm::mmap(virt_addr_t virt, size_t length, int protection, int flags) {
    critical_section cs;

    // We need to choose an address to return.
    virt_addr_t aligned_vaddr = get_virt_addr(virt & 0xFFFFF000, flags);

    flags |= VMM_FLAG_POPULATE;  // FIXME: Always set due to bugs. Fix.

    for (size_t i = 0; i < length; i += 0x1000) {
        if ((flags & VMM_FLAG_POPULATE) != 0) {
            phys_addr_t physpage = kernel::g_pmm.alloc_single(PMM_REGION_RAM);
            //kernel::log::trace("vmm", "mmap: 0x%08x->0x%08x\n", physpage, aligned_vaddr + i);
            if (!vas_current->map(physpage, aligned_vaddr + i, protection, flags)) { panic("Unable to map page"); }
        } else {
            //log::trace("vmm", "mmap: 0x%08x (on demand)\n", aligned_vaddr + i);
            if (!vas_current->map(0x0, aligned_vaddr + i, protection, flags)) { panic("Unable to map page"); }
        }
    }

    return (virt_addr_t*)aligned_vaddr;
}

virt_addr_t* virt_mm::mmap_direct(virt_addr_t virt, phys_addr_t phys, int protection, int flags) {
    critical_section cs;
    virt_addr_t      aligned_vaddr = get_virt_addr(virt & 0xFFFFF000, flags);

    flags |= VMM_FLAG_POPULATE;  // Direct MMAPs always will have the MAPNOW flag set

    vas_current->map(phys, virt, protection, flags);
    g_pmm.mark_used(phys);
    //log::trace("vmm", "mmap: 0x%08x->0x%08x\n", phys, aligned_vaddr);
    return (virt_addr_t*)aligned_vaddr;
}

/// Unmaps a region of virtual memory from a tasks virtual memory.
void virt_mm::munmap(virt_addr_t virt, size_t length) {
    critical_section cs;

    virt_addr_t aligned_vaddr = virt & 0xFFFFF000;
    for (size_t i = 0; i < length; i += 0x1000) { vas_current->unmap(aligned_vaddr + i); }
}

virt_addr_t virt_mm::get_virt_addr(virt_addr_t addr, int flags) {
    if (addr == 0) {
        if (flags & VMM_FLAG_KERNEL_REGION) { addr = 0xD0000000; }
        while (vas_current->get_page(addr).raw != 0) { addr += 0x1000; }
    }
    return addr;
}

void virt_mm::fulfill_demand_page(virt_addr_t virt) {
    virt_addr_t aligned_vaddr = virt & 0xFFFFF000;
    page_t      pg            = vas_current->get_page(aligned_vaddr);
    mmap(aligned_vaddr, 0x1000, (pg.write ? VMM_PROT_WRITE : 0) | (pg.user ? VMM_PROT_USER : 0), VMM_FLAG_POPULATE);
}
