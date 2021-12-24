#include <kernel/mm/memory.h>
#include <kernel/log.h>
namespace kernel {

virt_addr_t hhdm_vbase = 0xffff800000000000;
phys_addr_t kernel_pbase = 0xFFFFFFFF80000000;
phys_addr_t kernel_vbase = 0xFFFFFFFF80000000;
virt_addr_t phys_to_virt_addr(phys_addr_t addr) { return addr + hhdm_vbase; }
void        hhdm_set_vbase(virt_addr_t addr) { hhdm_vbase = addr; }

phys_addr_t virt_to_phys_addr(virt_addr_t addr) {
    if (addr >= kernel_vbase) {
        // This is a kernel address. Get the trivial physical address based on the pbase
        return (addr - kernel_vbase) + kernel_pbase;
    }
    return addr - hhdm_vbase; 
}
void        hhdm_set_kern_pbase(phys_addr_t addr) { kernel_pbase = addr;}
void        hhdm_set_kern_vbase(virt_addr_t addr) { kernel_vbase = addr; }
}  // namespace kernel
