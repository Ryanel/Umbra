#include <kernel/log.h>
#include <kernel/mm/pmm.h>

kernel::mm::pmm kernel::g_pmm;

using namespace kernel;
using namespace kernel::mm;

uintptr_t bitmap_arr[0x20000];  // HACK: Fix!

void pmm::init() {
    bitmap.granularity   = 0;
    bitmap.backing.array = (uintptr_t*)&bitmap_arr;
    bitmap.init(maxAddr);
}

void pmm::update_statistics() {
    ram_pages_used = 0;
    ram_pages_max  = 0;

    for (size_t i = 0; i < region_count; i++) {
        region& r = regions[i];
        if (regions[i].m_type != region::region_type::ram) { continue; }
        auto search_start = r.m_start;
        auto search_end   = r.m_end;

        for (phys_addr_t search = search_start; search < search_end; search += 0x1000) {
            if (bitmap.used(search)) { ram_pages_used++; }
            ram_pages_max++;
        }
    }
}

void pmm::print_statistics() {
    update_statistics();
    uint32_t used_kbytes = ram_pages_used * 4;  // KB;
    uint32_t free_kbytes = (ram_pages_max - ram_pages_used) * 4;
    uint32_t total_kbram = (ram_pages_max)*4;
    log::debug("pmm", "%d/%d pages used, %dkb used, %dkb free, %dkb max\n", ram_pages_used, ram_pages_max, used_kbytes,
               free_kbytes, total_kbram);

    /*
    for (unsigned int i = 0; i < region_count; i++) {
        region&     r     = regions[i];
        const char* rtype = "???";

        switch (r.m_type) {
            case region::region_type::ram: rtype = "ram"; break;

            default:
            case region::region_type::unmapped: rtype = "unm"; break;
        }
        log::trace("pmm", "region %d: %s, 0x%08x -> 0x%08x\n", i, rtype, r.m_start, r.m_end);
    }
    */
}

void pmm::add_region(region r) {
    regions[region_count] = r;
    region_count++;
}

phys_addr_t pmm::alloc_single(int flags) {
    // Search a region
    for (size_t i = 0; i < region_count; i++) {
        region&     r = regions[i];
        phys_addr_t search_start;
        phys_addr_t search_end;

        if (((flags & PMM_REGION_RAM) != 0) && regions[i].m_type != region::region_type::ram) {
            continue;
        } else {
            search_start = opt_firstFreeAddr;
            search_end   = r.m_end;
            if (search_start > search_end) { continue; }
            if (search_start < r.m_start) { search_start = r.m_start; }
        }

        for (phys_addr_t search = search_start; search < search_end; search += 0x1000) {
            if (bitmap.mark_if_clear(search)) {
                // Optimization to improve future searches. Start on the next page, as we know all
                // pages up to this page are allocated. In the linear case, this almost eliminates
                // searching with no freeing.
                opt_firstFreeAddr = search + 0x1000;
                ram_pages_used++;
                return search;
            }
        }
        // We weren't able to find any free memory... search for the next region
    }

    // Out of memory!
    return 0;
}

void pmm::free_single(phys_addr_t addr) {
    if (bitmap.used(addr)) {
        // Optimization: We know that the first free address is now addr, mark it.
        if (opt_firstFreeAddr > addr) { opt_firstFreeAddr = addr; }
        bitmap.free(addr);
        ram_pages_used--;
    }
}

phys_addr_t pmm::alloc_contiguous(int flags) { return 0; }