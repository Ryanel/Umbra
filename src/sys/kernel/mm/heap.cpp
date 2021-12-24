#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/mm/memory.h>

kheap g_heap;

virt_addr_t kheap::alloc(size_t sz, int flags, phys_addr_t* paddr) {
    if (m_full) {
        if ((flags & KHEAP_PAGEALIGN) != 0) {
            // Round up sz to the next page size. If a structure is going to be page aligned, SLAB guarentees that page
            // aligned allocation values will return page-aligned addresses.

            sz = (sz + PAGE_SIZE - 1) & (~(PAGE_SIZE - 1));

            // We also need to retrieve the physical address. We'll do this with a special allocator instead
            if ((flags & KHEAP_PHYSADDR) != 0 && paddr != nullptr) {
                return (virt_addr_t)slab_alloc.alloc(sz, flags, paddr);
            }
        }
        return (virt_addr_t)slab_alloc.alloc(sz, flags, nullptr);
    } else {
        // Align if requested
        if (((flags & KHEAP_PAGEALIGN) != 0) && ((early_placement & 0xFFFFFFFFFFFFF000) != early_placement)) {
            early_placement &= 0xFFFFFFFFFFFFF000;
            early_placement += 0x1000;
        }

        auto newAddr = early_placement;
        auto phys    = kernel::virt_to_phys_addr(newAddr);

        if ((flags & KHEAP_PHYSADDR) != 0 && paddr != nullptr) { *paddr = phys; }

        // okay, we need to make sure this is mapped...
        if (!kernel::g_pmm.used(phys)) {
            // It's not, map it...
            for (uintptr_t i = 0; i < sz; i += 0x1000) {
                kernel::log::trace("kheap", "grew a page @ 0x%016p\n", phys + i);
                kernel::g_vmm.mmap_direct(early_placement + i, phys + i, VMM_PROT_WRITE, VMM_FLAG_POPULATE);
            }
        }

        kernel::log::trace("kheap", "alloc %d bytes @ 0x%08x flags %x\n", sz, newAddr, flags);
        early_placement += sz;
        return newAddr;
    }

    return 0;
}

void kheap::free(virt_addr_t addr) {
    if (m_full) {
        slab_alloc.free((void*)addr);
    } else {
        // We can't free things from the early allocator!
    }
}

void kheap::init(bool full, uintptr_t placement_addr) {
    this->m_full = full;
    if (full) {
        early_placement &= 0xFFFFFFFFFFFFF000;
        early_placement += 0x1000;
        slab_alloc.init(early_placement, 0x100000);  // TODO: Define a max heap size!
    } else {
        early_placement = placement_addr;
        if (early_placement & 0xFFFFFFFFFFFFF000) {
            early_placement &= 0xFFFFFFFFFFFFF000;
            early_placement += 0x1000;
        }
    }
}

void kheap::debug() {
    if (m_full) {
        kernel::log::debug("heap", "Heap is in full mode\n");
        slab_alloc.debug();
    } else {
        kernel::log::debug("heap", "Heap is in early mode\n");
    }
}

void* operator new(size_t size) { return (void*)g_heap.alloc(size, 0x0); }
void* operator new[](size_t size) { return (void*)g_heap.alloc(size, 0x0); }
void  operator delete(void* p) { g_heap.free((virt_addr_t)p); }
void  operator delete(void* p, size_t size) { g_heap.free((virt_addr_t)p); }
void  operator delete[](void* p) { g_heap.free((virt_addr_t)p); }
void  operator delete[](void* p, size_t sz) { g_heap.free((virt_addr_t)p); }