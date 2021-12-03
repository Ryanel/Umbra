#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>

kheap g_heap;

virt_addr_t kheap::alloc(size_t sz, int flags, phys_addr_t* paddr) {
    if (m_full) {
        // TODO: Fix this is very broken!
        if ((flags & KHEAP_PAGEALIGN) != 0) {
            // Revert to early
            early_placement = slab_alloc.m_heap;

            if ((early_placement & 0xFFFFF000)) {
                early_placement &= 0xFFFFF000;
                early_placement += 0x1000;
            }

            phys_addr_t phys = kernel::g_pmm.get_available_page();
            kernel::g_vmm.mmap_direct(early_placement, phys, 0x03);

            if ((flags & KHEAP_PHYSADDR) != 0 && paddr != nullptr) { *paddr = phys; }

            kernel::log::error("heap", "Returning 0x%08x @ 0x%08x\n", early_placement, phys);
            slab_alloc.m_heap = (early_placement + sz);
            return early_placement;
        }

        return (virt_addr_t)slab_alloc.alloc(sz);
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
                kernel::log::trace("kheap", "grew a page @ 0x%08x\n", phys + i);
                kernel::g_vmm.mmap_direct(early_placement + i, phys + i, 0x03);
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
        early_placement &= 0xFFFFF000;
        early_placement += 0x1000;
        slab_alloc.init(early_placement, 0x100000);  // TODO: Define a max heap size!
    } else {
        this->early_placement = placement_addr;
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