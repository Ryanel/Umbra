#include <kernel/x86/paging.h>

void paging_node::describe(uint32_t virt) {
    
}
uint32_t paging_node::current_get(uint32_t virt) {
    unsigned long pdindex = (unsigned long)virt >> 22;
    unsigned long ptindex = (unsigned long)virt >> 12 & 0x03FF;

    unsigned long* pd = (unsigned long*)0xFFFFF000;
    // Here you need to check whether the PD entry is present.

    unsigned long* pt = ((unsigned long*)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.

    return (uint32_t)((pt[ptindex] & ~0xFFF) + ((unsigned long)virt & 0xFFF));
}

void paging_node::current_map(uint32_t phys, uint32_t virt, uint32_t flags) {
    // Make sure that both addresses are page-aligned.

    unsigned long pdindex = (unsigned long)virt >> 22;
    unsigned long ptindex = (unsigned long)virt >> 12 & 0x03FF;

    unsigned long* pd = (unsigned long*)0xFFFFF000;
    // Here you need to check whether the PD entry is present.
    // When it is not present, you need to create a new empty PT and
    // adjust the PDE accordingly.

    unsigned long* pt = ((unsigned long*)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.
    // When it is, then there is already a mapping present. What do you do now?

    pt[ptindex] = ((unsigned long)phys) | (flags & 0xFFF) | 0x01;  // Present

    // Now you need to flush the entry in the TLB
    // or you might not notice the change.
    flush_tlb_single(phys);
}

void paging_node::current_unmap(uint32_t virt) {

}