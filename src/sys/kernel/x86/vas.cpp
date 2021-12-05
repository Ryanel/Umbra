#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/x86/vas.h>
#include <string.h>

namespace kernel {

bool vas::map(phys_addr_t phys, virt_addr_t virt, uint32_t proto, int flags) {
    // Make sure that both addresses are page-aligned.
    unsigned long pdindex = (unsigned long)virt >> 22;
    unsigned long ptindex = (unsigned long)virt >> 12 & 0x03FF;

    auto* pd_ent = &directory->entries[pdindex];

    // Check if the PD entry exists.
    if (pd_ent->present != 1) {
        phys_addr_t new_page_table_phys;

        auto new_pt_virt = g_heap.alloc(0x1000, KHEAP_PAGEALIGN | KHEAP_PHYSADDR, &new_page_table_phys);
        memset((void*)new_pt_virt, 0, 0x1000);

        kernel::log::debug("map", "No page table, allocated a new one at %x\n", new_pt_virt);
        // if (new_pt_virt == -1) { return false; }

        this->pt_virt[pdindex]          = new_pt_virt;
        directory->entries[pdindex].raw = (new_page_table_phys | VAS_PRESENT | VAS_WRITABLE);
    }

    auto* pt = (page_table_t*)pt_virt[pdindex];

    bool demand_paging       = (flags & VMM_FLAG_POPULATE) == 0;
    int  filtered_flags      = (proto & 0xFFF) | (demand_paging ? VAS_DEMAND_MAPPED : VAS_PRESENT);
    pt->entries[ptindex].raw = ((unsigned long)phys) | filtered_flags;

    tlb_flush_single(phys);
    return true;
}

bool vas::unmap(virt_addr_t virt) {
    unsigned long pdindex = (unsigned long)virt >> 22;
    unsigned long ptindex = (unsigned long)virt >> 12 & 0x03FF;
    auto*         pd_ent  = &directory->entries[pdindex];

    // Check if the PD entry exists.
    if (pd_ent->present != 1) { return false; }

    auto* pt = (page_table_t*)pt_virt[pdindex];

    pt->entries[ptindex].raw = 0;

    tlb_flush_single(virt);
    return true;
}

page_t vas::get_page(uintptr_t virt) {
    uintptr_t aligned_addr = virt & 0xFFFFF000;

    unsigned long pdindex = (unsigned long)aligned_addr >> 22;
    unsigned long ptindex = (unsigned long)aligned_addr >> 12 & 0x03FF;

    auto* pd_ent = &directory->entries[pdindex];
    if (pd_ent->present == 1) {
        auto* pt = (page_table_t*)pt_virt[pdindex];
        return pt->entries[ptindex];
    }

    page_t pg;
    pg.raw = 0;
    return pg;
}

bool vas::has_table(uintptr_t virt) {
    uintptr_t aligned_addr = virt & 0xFFFFF000;

    unsigned long pdindex = (unsigned long)aligned_addr >> 22;
    unsigned long ptindex = (unsigned long)aligned_addr >> 12 & 0x03FF;

    auto* pd_ent = &directory->entries[pdindex];
    return pd_ent->present == 1;
}

vas* vas::clone() {
    // Allocate the page directory metadata structure from the heap
    auto pd_meta = g_heap.alloc(0x1000, 0);
    vas* dir     = (vas*)pd_meta;

    // Now, allocate the actual page directory read by the CPU
    phys_addr_t pd_phys;
    auto        pd_virt = g_heap.alloc(0x1000, KHEAP_PAGEALIGN | KHEAP_PHYSADDR, &pd_phys);
    memset((void*)pd_virt, 0, 0x1000);

    // Setup the meta directory
    dir->directory      = (page_directory_raw_t*)pd_virt;
    dir->directory_addr = pd_phys;

    kernel::log::debug("pg", "Cloning page directory 0x%08x to 0x%08x\n", this->directory_addr, dir->directory_addr);

    // Now, fill in the page table
    // bool               cow                = false;  // Clone pages as copy on write. COW is disabled for kernel pages
    bool               kernel_only        = true;  // Clone only the kernel
    unsigned const int kernel_start_index = 768;   // 0xC0000000 -> 768
    unsigned int       start_index        = kernel_only ? kernel_start_index : 0;

    // Map entries
    for (size_t i = start_index; i < 1024; i++) {
        bool link = false;
        // Don't worry aboyut empty entries
        if (this->directory->entries[i].raw == 0) { continue; }
        // Link everything above this point
        if (i >= kernel_start_index) { link = true; }

        if (link) {
            dir->directory->entries[i] = this->directory->entries[i];
            dir->pt_virt[i]            = this->pt_virt[i];
            kernel::log::trace("pg", "Link %d (p:0x%08x)\n", i, this->directory->entries[i].address * 0x1000);
        } else {
            panic("Todo, write copying for page tables");
        }
    }

    return dir;
}

}  // namespace kernel