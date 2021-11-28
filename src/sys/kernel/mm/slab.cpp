#include <kernel/log.h>
#include <kernel/mm/slab.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <stdlib.h>

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
    bool  alloc     = slab_meta.alloc(sizeof(slab), slab_self_meta);
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
        // TODO: Panic
    }
    return original_heap;
}

void* slab_allocator::alloc(uint32_t size) {
    uintptr_t out_addr;

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
    if (ptr == nullptr) {
        panic("Attempted to free a null pointer");
        return;
    }
    for (slab* s = slab_last_allocated; s; s = s->m_next) {
        if (s->free((uintptr_t)ptr)) { return; }
    }

    // Uh, this pointer does not exist.
    panic("Attempted to free a pointer that has not been allocated.");
}

void slab_allocator::debug() {
    klogf("slab", "slab allocator: start @ %08x, current is %08x, max is %08x\n", m_heap_start, m_heap, m_heap_max);
    for (slab* s = slab_last_allocated; s; s = s->m_next) { s->debug(); }
}

void slab::init(uintptr_t start, uint32_t sz) {
    if (sz < sizeof(uintptr_t)) {
        klogf("slab", "Attempted alloc of size < %u. Raised to minimum size.\n", sizeof(uintptr_t));
        sz = sizeof(uintptr_t);
    }

    m_next    = nullptr;
    m_start   = start;
    m_size    = sz;
    m_entries = 0;

    // Determine how many pages to allocate here.
    m_pages      = get_pages(sz);
    m_maxEntries = ((PAGE_SIZE * m_pages) / m_size);

    // vmm: Map the used pages here
    kernel::g_vmm.mmap(start, 0x1000 * m_pages, 0x3);  // TODO: Allow other types of protection

    // Determine how many entires we can have
    //klogf("slab", "Creating new slab @ 0x%08x with size %d, and %d entries in %d pages\n", start, sz, m_maxEntries, m_pages);

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
    out_address = (uintptr_t)m_free_list;
    m_free_list = m_free_list->m_next;
    m_entries++;
    return true;
}

bool slab::free(uintptr_t address) {
    // This address is not within this slab!
    if (address < m_start || address >= m_start + (PAGE_SIZE * m_pages)) { return false; }
    if (m_entries == 0) { return false; }
    // TODO: Check that address is size aligned

    // Put this address on the free list.
    auto* entry   = (slab_entry*)address;
    entry->m_next = m_free_list;
    m_free_list   = entry;
    m_entries--;
    return true;
}

void slab::debug() {
    klogf("slab", "%08x: sz:%-4d bytes; %3d/%-3d used; %d pages\n", m_start, m_size, m_entries, m_maxEntries, m_pages);
}