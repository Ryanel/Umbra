#include <kernel/x86/paging.h>

bool page_directory::map(phys_addr_t phys, virt_addr_t virt, uint32_t flags) {
    // Make sure that both addresses are page-aligned.
    unsigned long pdindex = (unsigned long)virt >> 22;
    unsigned long ptindex = (unsigned long)virt >> 12 & 0x03FF;

    page_directory_entry_t* pd_ent = &directory->entries[pdindex];

    // Check if the PD entry exists.
    if (pd_ent->present != 1) {
        klogf("map", "No page table...\n");
        return false;
    }

    page_table_t* pt         = (page_table_t*)pt_virt[pdindex];
    pt->entries[ptindex].raw = ((unsigned long)phys) | (flags & 0xFFF) | 0x01;

    tlb_flush_single(phys);
    return true;
}

bool page_directory::unmap(virt_addr_t virt) {
    unsigned long           pdindex = (unsigned long)virt >> 22;
    unsigned long           ptindex = (unsigned long)virt >> 12 & 0x03FF;
    page_directory_entry_t* pd_ent  = &directory->entries[pdindex];

    // Check if the PD entry exists.
    if (pd_ent->present != 1) { return false; }

    page_table_t* pt         = (page_table_t*)pt_virt[pdindex];
    pt->entries[ptindex].raw = 0;

    tlb_flush_single(virt);
}
