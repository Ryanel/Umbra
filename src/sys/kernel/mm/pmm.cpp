#include <kernel/log.h>
#include <kernel/mm/pmm.h>
kernel::phys_mm kernel::g_pmm;

uintptr_t bitmap_arr[0x20000];

void kernel::phys_mm::add_region(pmm_region_t region) {
    m_regions[m_region_count] = region;
    m_region_count++;
}

void kernel::phys_mm::init() {
    backing_store.array      = bitmap_arr;
    backing_store.size_bytes = 0x20000;

    m_available_pages = 0x0000;

    for (short i = 0; i < m_region_count; i++) {
        auto&     reg        = m_regions[i];
        uintptr_t start_page = (reg.m_start & 0xFFFFF000) / 0x1000;
        uintptr_t end_page   = (reg.m_end & 0xFFFFF000) / 0x1000;

        for (uintptr_t j = start_page; j < end_page; j++) {
            if (reg.m_type == pmm_region_type::ram) {
                backing_store.set(j);
                m_available_pages++;
            }
        }
    }

    m_max_available_pages = m_available_pages;
    kernel::log::debug("pmm", "RAM: 0x%08x, or %d MB\n", m_available_pages * 0x1000,
                       (m_available_pages * 0x1000) / 1024 / 1024);
}

bool kernel::phys_mm::page_available(phys_addr_t addr) { return backing_store.test((addr & 0xFFFFF000) / 0x1000); }

void kernel::phys_mm::describe() const {
    kernel::log::debug("pmm", "regions: %d\n", m_region_count);
    const char* type_unknown = "unknown";
    const char* type_ram     = "ram";

    for (short i = 0; i < m_region_count; i++) {
        auto&       reg  = m_regions[i];
        const char* type = type_unknown;

        if (reg.m_type == pmm_region_type::ram) { type = type_ram; }
        kernel::log::debug("pmm", "%d | %10s | 0x%08x -> 0x%08x\n", i, type, reg.m_start, reg.m_end);
    }
}

void kernel::phys_mm::mark_free(phys_addr_t addr) {
    uintptr_t idx = (addr & 0xFFFFF000) / 0x1000;
    backing_store.set(idx);
}

void kernel::phys_mm::mark_used(phys_addr_t addr) {
    uintptr_t idx = (addr & 0xFFFFF000) / 0x1000;
    backing_store.clear(idx);
}

phys_addr_t kernel::phys_mm::get_available_page() {
    uintptr_t end_page = (0xFFFFF000) / 0x1000;

    for (uintptr_t i = 0; i < end_page; i++) {
        if (backing_store.test(i)) {
            backing_store.clear(i);
            m_available_pages--;
            return i * 0x1000;
        }
    }
    return -1;
}
