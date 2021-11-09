#include <kernel/mm/heap.h>
#include <kernel/x86/paging.h>
#include <string.h>
bool page_directory::map(phys_addr_t phys, virt_addr_t virt, uint32_t flags) {
    // Make sure that both addresses are page-aligned.
    unsigned long pdindex = (unsigned long)virt >> 22;
    unsigned long ptindex = (unsigned long)virt >> 12 & 0x03FF;

    page_directory_entry_t* pd_ent = &directory->entries[pdindex];

    // Check if the PD entry exists.
    if (pd_ent->present != 1) {
        phys_addr_t new_page_table_phys;

        virt_addr_t new_pt_virt = g_heap.alloc(0x1000, KHEAP_PAGEALIGN | KHEAP_PHYSADDR, &new_page_table_phys);
        memset((void*)new_pt_virt, 0, 0x1000);

        klogf("map", "No page table, allocated a new one at %x\n", new_pt_virt);
        //if (new_pt_virt == -1) { return false; }

        this->pt_virt[pdindex]          = new_pt_virt;
        directory->entries[pdindex].raw = (new_page_table_phys | 0x03);
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
    return true;
}
