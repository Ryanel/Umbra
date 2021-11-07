#include <kernel/mm/pmm.h>
#include <kernel/log.h>
kernel::phys_mm kernel::g_pmm;

void kernel::phys_mm::add_region(pmm_region_t region) {
    regions[region_count] = region;
    region_count++;
}

void kernel::phys_mm::describe() const {
    klogf("pmm","regions: %d\n", region_count);
}