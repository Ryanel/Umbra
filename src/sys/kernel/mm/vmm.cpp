#include <kernel/scheduler.h>
#include <kernel/log.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>

kernel::virt_mm kernel::g_vmm;

virt_addr_t* kernel::virt_mm::mmap(virt_addr_t virt, size_t length, int protection) {
    kernel::critical_section cs;
    virt_addr_t      aligned_vaddr = virt & 0xFFFFF000;

    for (size_t i = 0; i < length; i += 0x1000) {
        if ((protection & VMM_MMAP_FLAG_MAPNOW) != 0) {
            phys_addr_t physpage = kernel::g_pmm.get_available_page();
            kernel::log::trace("vmm", "mmap: 0x%08x->0x%08x\n", physpage, aligned_vaddr + i);
            if (!vas_current->map(physpage, aligned_vaddr + i, protection)) { panic("Unable to map page"); }
        } else {
            if (!vas_current->map(0x0, aligned_vaddr + i, protection)) { panic("Unable to map page"); }
        }
    }

    return (virt_addr_t*)aligned_vaddr;
}

virt_addr_t* kernel::virt_mm::mmap_direct(virt_addr_t virt, phys_addr_t phys, int protection) {
    kernel::critical_section cs;
    virt_addr_t aligned_vaddr = virt & 0xFFFFF000;
    protection |= 0x1;
    vas_current->map(phys, virt, protection);
    g_pmm.mark_used(phys);
    kernel::log::trace("vmm", "mmap: 0x%08x->0x%08x\n", phys, aligned_vaddr);
    return (virt_addr_t*)aligned_vaddr;
}

/// Unmaps a region of virtual memory from a tasks virtual memory.
void kernel::virt_mm::munmap(virt_addr_t virt, size_t length) {
    kernel::critical_section cs;

    virt_addr_t aligned_vaddr = virt & 0xFFFFF000;
    for (size_t i = 0; i < length; i += 0x1000) { vas_current->unmap(aligned_vaddr + i); }
}

void kernel::virt_mm::fulfill_demand_page(virt_addr_t virt) {
    virt_addr_t aligned_vaddr = virt & 0xFFFFF000;
    page_t      pg            = vas_current->get_page(aligned_vaddr);
    mmap(aligned_vaddr, 0x1000, VMM_MMAP_FLAG_MAPNOW | (pg.write ? VMM_MMAP_WRITABLE : 0) | (pg.user ? VMM_MMAP_USERMODE : 0));
}