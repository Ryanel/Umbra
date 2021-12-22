#include <kernel/mm/memory.h>

namespace kernel {

virt_addr_t hhdm_vbase = 0xffff800000000000;

virt_addr_t phys_to_virt_addr(phys_addr_t addr) { return addr + hhdm_vbase; }
void        hhdm_set_virtbase(virt_addr_t addr) { hhdm_vbase = addr; }
phys_addr_t virt_to_phys_addr(virt_addr_t addr) { return addr - hhdm_vbase; }
void        hhdm_set_kernphysbase(phys_addr_t addr) {}
}  // namespace kernel