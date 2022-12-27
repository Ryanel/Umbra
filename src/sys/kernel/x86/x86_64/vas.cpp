#include <assert.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/memory.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/x86/vas.h>
#include <string.h>
#include <kernel/config.h>

// Physical memory is mapped to 0xffff000000000000 and beyond.
// Kernel Memory is mapped to 0xffffffff80000000, and the heap is located in this 2GB region

// Usage:
// Kernel manipulates memory using the memory::virtual_to_pvirt() call for things that should use physical.
// Physical
// Page tables can be easily translated this way and mapped in a VAS-compatible way, without a seperate table

namespace kernel {

bool vas::map(phys_addr_t phys, virt_addr_t virt, uint32_t proto, int flags) {
    if ((flags & VMM_FLAG_POPULATE) != 0) {
        proto |= VAS_PRESENT;
    } else {
        proto |= VAS_DEMAND_MAPPED;
    }

    if ((flags & VAS_HUGE_PAGE) != 0) {
        vas::table_info info        = get_table(virt, true, 2);
        info.ptr->entries[info.idx] = phys | proto | VAS_HUGE_PAGE;
        return true;
    }

    vas::table_info info = get_table(virt, true, 1);

    info.ptr->entries[info.idx] = phys | proto;
    tlb_flush_single(virt);
    return true;
}
bool vas::unmap(virt_addr_t virt) { 
    vas::table_info info = get_table(virt, true, 1);
    info.ptr->entries[info.idx] = 0;
    tlb_flush_single(virt);
    return true;
}

page_t vas::get_page(uintptr_t virt) {
    auto tab = get_table(virt, false);
    return tab.ptr->entries[tab.idx];
}

bool vas::has_table(uintptr_t virt) { return false; }
vas* vas::clone() {
    // Allocate the page directory metadata structure from the heap
    auto pd_meta = g_heap.alloc(PAGE_SIZE, KHEAP_PAGEALIGN);
    vas* dir     = (vas*)pd_meta;

    // Now, allocate the actual page directory read by the CPU
    phys_addr_t pd_phys;
    auto        pd_virt = g_heap.alloc(PAGE_SIZE, KHEAP_PAGEALIGN | KHEAP_PHYSADDR, &pd_phys);
    memset((void*)pd_virt, 0, PAGE_SIZE);

    // Setup the meta directory
    dir->directory      = (pml_t*)pd_virt;
    dir->directory_addr = pd_phys;

    kernel::log::debug("pg", "Cloning page directory 0x%08x to 0x%08x\n", this->directory_addr, dir->directory_addr);

    // Now, fill in the page table
    // bool               cow                = false;  // Clone pages as copy on write. COW is disabled for kernel pages
    bool               kernel_only        = true;  // Clone only the kernel
    unsigned const int kernel_start_index = 256;
    unsigned int       start_index        = kernel_only ? kernel_start_index : 0;

    // Map entries
    for (size_t i = start_index; i < 512; i++) {
        bool link = false;
        // Don't worry aboyut empty entries
        if (this->directory->entries[i] == 0) { continue; }

        // Link everything above this point
        if (i >= kernel_start_index) { link = true; }

        if (link) {
            dir->directory->entries[i] = this->directory->entries[i];
        } else {
            panic("Todo, write copying for page tables");
        }
    }

    return dir;
}
bool vas::create_table(uintptr_t vaddr) { return false; }

vas::table_info vas::get_table(virt_addr_t addr, bool create, unsigned int desiredLevel) {
    constexpr size_t max_level = 4;  // We don't support PML5 yet
    unsigned long    indicies[max_level + 1];
    size_t           current_level = max_level;

    addr &= 0xFFFFFFFFFFFFF000;

    // Extract PML indicies from virtual address
    indicies[4] = (unsigned long)(addr >> 39) & 511;
    indicies[3] = (unsigned long)(addr >> 30) & 511;
    indicies[2] = (unsigned long)(addr >> 21) & 511;
    indicies[1] = (unsigned long)(addr >> 12) & 511;

    // Going from PML to PML descending, get the next entry. Creates PMLs if create=true
    assert(directory != nullptr);

    table_info info;
    info.ptr = this->directory;

    for (size_t current_level = max_level; current_level > desiredLevel; current_level--) {
        size_t cur_idx = indicies[current_level];
        assert(info.ptr != nullptr);

        info.level       = current_level;
        vas_entry_t next = info.ptr->entries[cur_idx];

        // Now, check if 2MB paging is enabled. If it is, simply return current
        if ((next & (1 << 7)) != 0) {
            info.large_page = true;
            info.level      = info.level - 1;
            info.ptr        = (pml_t*)kernel::phys_to_virt_addr(next & 0xFFFFFFFFFFFFF000);
            return info;
        }

        // Check if the next table exists...
        if (next == 0 && create) {
            phys_addr_t phys = kernel::g_pmm.alloc_single(0);
            virt_addr_t virt = kernel::phys_to_virt_addr(phys);

            info.ptr->entries[cur_idx] = phys | 0x03;  // Page tables are always present
            next                       = info.ptr->entries[cur_idx];
        } else if (next == 0) {  // Don't create, simply return
            info.ptr = nullptr;
            return info;
        }

        info.ptr = (pml_t*)kernel::phys_to_virt_addr(next & 0xFFFFFFFFFFFFF000);
    }

    info.idx = indicies[info.level - 1];

    return info;
}

}  // namespace kernel