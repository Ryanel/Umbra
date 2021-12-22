#pragma once

#include <kernel/types.h>

namespace kernel {

virt_addr_t phys_to_virt_addr(phys_addr_t addr);
phys_addr_t virt_to_phys_addr(virt_addr_t addr);
void        hhdm_set_kernphysbase(phys_addr_t addr);
void        hhdm_set_virtbase(virt_addr_t addr);

}  // namespace kernel