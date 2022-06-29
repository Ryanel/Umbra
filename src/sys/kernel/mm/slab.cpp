#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/slab.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/tasks/critical_section.h>
#include <stdlib.h>
#include <string.h>

using namespace kernel;
using namespace kernel::tasks;

uintptr_t power_ceil(uintptr_t x) {
    if (x <= 1) return 1;
    int power = 2;
    x--;
    while (x >>= 1) power <<= 1;
    return power;
}

slab* slab_allocator::slab_alloc_meta() {
    slab slab_meta;

    uintptr_t meta_addr = allocate_heap(slab::get_pages(sizeof(slab)) * PAGE_SIZE);

    slab_meta.init(meta_addr, sizeof(slab));

    // Store the metadata of this slab within this slab.
    slab* slab_self_meta;
    slab_meta.alloc(sizeof(slab), slab_self_meta);
    *slab_self_meta = slab_meta;  // Copy the structure into main memory.
    return slab_self_meta;        // Return the pointer. This should be equal to meta_addr.
}

void slab_allocator::init(uintptr_t v_memstart, uintptr_t vmem_size) {
    m_heap_start = v_memstart;
    m_heap       = v_memstart;
    m_heap_max   = v_memstart + vmem_size;

    slab_meta_head      = slab_alloc_meta();
    slab_last_allocated = slab_meta_head;
}

uintptr_t slab_allocator::allocate_heap(uint32_t size) {
    size                    = (size & ~(PAGE_SIZE - 1));  // Align to page size
    uintptr_t original_heap = m_heap;

    if ((m_heap + size) < m_heap_max) {
        m_heap += size;
    } else {
        panic("Ran out of heap memory");
    }
    return original_heap;
}

void* slab_allocator::alloc(uint32_t size, int flags, phys_addr_t* addr) {
    critical_section cs;
    uintptr_t        out_addr = 0;

    if (size < SLAB_MIN_OBJSIZE) { size = SLAB_MIN_OBJSIZE; }

    // This is preliminary support. We can't free this either, which will lead to errors.
    if (flags & KHEAP_PHYSADDR) {
        out_addr = allocate_heap(size);
        if (size > 0x1000) { panic("Unimplemented"); }
        *addr   = g_pmm.alloc_single(PMM_REGION_RAM);
        void* x = g_vmm.mmap_direct(out_addr, *addr, VMM_PROT_WRITE, flags);
        return x;
    }

    // Search for a free size-appropreate slab.
    for (slab* s = slab_last_allocated; s; s = s->m_next) {
        if (s->alloc(size, out_addr)) { return (void*)out_addr; }
    }

    // There is no existing slab. Create one.
    slab* out_slab;
    bool  slab_alloc_success = slab_meta_head->alloc(sizeof(slab), out_slab);

    if (!slab_alloc_success) {
        slab* slab_meta = slab_alloc_meta();

        slab_meta->m_next   = slab_last_allocated;
        slab_last_allocated = slab_meta;
        slab_meta_head      = slab_meta;

        slab_meta_head->alloc(sizeof(slab), out_slab);
    }

    // Init the new slab
    out_slab->init(allocate_heap(slab::get_pages(size) * PAGE_SIZE), size);
    out_slab->m_next    = slab_last_allocated;
    slab_last_allocated = out_slab;

    // Allocate from this slab
    out_slab->alloc(size, out_addr);
    return (void*)out_addr;
}

void slab_allocator::free(void* ptr) {
    critical_section cs;

    if (ptr == nullptr) {
        panic("Attempted to free a null pointer");
        return;
    }

    for (slab* s = slab_last_allocated; s; s = s->m_next) {
        if (s->free((uintptr_t)ptr)) {
            // log::trace("slab", "Freed 0x%08x (%d bytes)\n", (uintptr_t)ptr, s->m_size);
            return;
        }
    }

    // Uh, this pointer does not exist.
    panic("Attempted to free a pointer that has not been allocated.");
}

void slab_allocator::debug() {
    log::debug("slab", "slab allocator: start @ %016p, current is %016p, max is %016p\n", m_heap_start, m_heap,
               m_heap_max);
    for (slab* s = slab_last_allocated; s; s = s->m_next) { s->debug(); }
}

void slab::init(uintptr_t start, uint32_t sz) {
    if (sz < sizeof(uintptr_t)) {
        log::error("slab", "Attempted alloc of size < %u. Raised to minimum size.\n", sizeof(uintptr_t));
        sz = sizeof(uintptr_t);
    }

    m_next    = nullptr;
    m_start   = start;
    m_size    = sz;
    m_entries = 0;

    m_pages      = get_pages(sz);                                 // Determine the number of pages
    m_maxEntries = (uint16_t)((PAGE_SIZE * m_pages) / m_size);    // Determine the # of entries
    g_vmm.mmap(m_start, PAGE_SIZE * m_pages, VMM_PROT_WRITE, 0);  // Map the used pages
    memset((void*)m_start, 0, PAGE_SIZE * m_pages);               // Clear the entry area

    // Populate the slabs free list
    m_free_list       = (slab_entry*)m_start;
    auto* current_ent = m_free_list;

    for (size_t i = 0; i < m_maxEntries; i++) {
        current_ent->m_next = (slab_entry*)(m_start + (i * m_size));
        current_ent         = current_ent->m_next;
    }
}

bool slab::alloc(uintptr_t sz, uintptr_t& out_address) {
    if (sz != m_size || m_free_list == nullptr) { return false; }
    if (m_entries >= m_maxEntries) { return false; }
    out_address = (uintptr_t)m_free_list;
    m_free_list = m_free_list->m_next;
    m_entries++;
    return true;
}

bool slab::free(uintptr_t address) {
    // This address is not within this slab!
    if (address < m_start || address >= m_start + (PAGE_SIZE * m_pages)) { return false; }
    if (m_entries == 0) { return false; }

    // Check that address is size aligned to the size of object with this slab!
    // We can do this because the slab is aligned to PAGE_SIZED
    uintptr_t offset = address - (address & 0xfffff000);
    if ((offset % this->m_size) != 0) {
        panic("Attempted heap corruption, offset to free is not a multiple of slab size!");
    }

    // Put this address on the free list.
    auto* entry   = (slab_entry*)address;
    entry->m_next = m_free_list;
    m_free_list   = entry;
    m_entries--;
    return true;
}

void slab::debug() {
    log::debug("slab", " - %08x: sz:%-4d %d/%d; pg:%d\n", m_start & 0x00000000FFFFFFFF, m_size, m_entries, m_maxEntries,
               m_pages);
}