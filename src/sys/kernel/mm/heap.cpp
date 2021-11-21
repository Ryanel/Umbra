#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>

kheap g_heap;

virt_addr_t kheap::alloc(size_t sz, int flags, phys_addr_t* paddr) {
    if (full) {
    } else {
        // Align if requested
        if ((flags & KHEAP_PAGEALIGN) != 0 && (early_placement & 0xFFFFF000)) {
            early_placement &= 0xFFFFF000;
            early_placement += 0x1000;
        }

        auto newAddr = early_placement;
        auto phys    = newAddr - 0xC0000000;

        if ((flags & KHEAP_PHYSADDR) != 0 && paddr != nullptr) { *paddr = phys; }

        // okay, we need to make sure this is mapped...
        if (kernel::g_pmm.page_available(phys)) {
            // It's not, map it...
            for (size_t i = 0; i < sz; i += 0x1000) {
                klogf("kheap", "grew a page @ 0x%08x\n", phys + i);
                kernel::g_vmm.mmap_direct(early_placement + i, phys + i, 0x03);
            }
        }

        klogf("kheap", "alloc %d bytes @ 0x%08x flags %x\n", sz, newAddr, flags);
        early_placement += sz;
        return newAddr;
    }

    return 0;
}

void kheap::free(virt_addr_t addr) {
    if (full) {
        // TODO
    } else {
        // We can't free things from the early allocator!
    }
}

void kheap::init(bool full, uintptr_t placement_addr) {
    this->full = full;
    if (full) {
    } else {
        this->early_placement = placement_addr;
    }
}

void* operator new(size_t size) { return (void*)g_heap.alloc(size, 0x0); }
void* operator new[](size_t size) { return (void*)g_heap.alloc(size, 0x0); }
void  operator delete(void* p) { g_heap.free((virt_addr_t)p); }
void  operator delete[](void* p) { g_heap.free((virt_addr_t)p); }