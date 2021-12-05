#include <kernel/critical.h>
#include <kernel/log.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>

kernel::virt_mm kernel::g_vmm;

virt_addr_t* kernel::virt_mm::mmap(virt_addr_t virt, size_t length, int protection) {
    critical_section cs;
    virt_addr_t      aligned_vaddr = virt & 0xFFFFF000;

    for (size_t i = 0; i < length; i += 0x1000) {
        phys_addr_t physpage = kernel::g_pmm.get_available_page();
        kernel::log::trace("vmm", "mmap: 0x%08x->0x%08x\n", physpage, aligned_vaddr + i);
        if (!vas_current->map(physpage, aligned_vaddr + i, protection)) { panic("Unable to map page"); }
    }

    return (virt_addr_t*)aligned_vaddr;
}

virt_addr_t* kernel::virt_mm::mmap_direct(virt_addr_t virt, phys_addr_t phys, int protection) {
    critical_section cs;

    virt_addr_t aligned_vaddr = virt & 0xFFFFF000;
    vas_current->map(phys, virt, protection);
    g_pmm.mark_used(phys);
    kernel::log::trace("vmm", "mmap: 0x%08x->0x%08x\n", phys, aligned_vaddr);
    return (virt_addr_t*)aligned_vaddr;
}

/// Unmaps a region of virtual memory from a tasks virtual memory.
void kernel::virt_mm::munmap(virt_addr_t virt, size_t length) {
    critical_section cs;

    virt_addr_t aligned_vaddr = virt & 0xFFFFF000;
    for (size_t i = 0; i < length; i += 0x1000) { vas_current->unmap(aligned_vaddr + i); }
}