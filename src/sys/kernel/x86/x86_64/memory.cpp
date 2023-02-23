#include <assert.h>
#include <kernel/boot/boot_file.h>
#include <kernel/boot/stivale2.h>
#include <kernel/hal/fb_text_console.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/memory.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/x86/serial_text_console.h>
#include <kernel/x86/vas.h>
#include <kernel/x86/vga_text_console.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <algorithm>

using namespace kernel;

#define MMAP_TYPE_RAM 0x1

extern kernel::device::fb_text_console con_fb;

extern "C" void set_page_table(uint64_t ptr);

kernel::vas kernel_vas;


/// Coordinates the initialization of all memory systems of the kernel
/// This function:
/// 1. Initialises the physical memory manager
/// 2. Initialises the kernel's page table.
/// 3. Initialises the virtual memory manager
/// 4. Initialises the heap
void boot_init_memory(struct stivale2_struct* svs) {
    uint64_t max_ram_addr = 0;

    auto* hhdm_tag = (stivale2_struct_tag_hhdm*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_HHDM_ID);
    auto* mmap_tag = (stivale2_struct_tag_memmap*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    auto* pmr_tag  = (stivale2_struct_tag_pmrs*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_PMRS_ID);
    auto* kbas_tag = (stivale2_struct_tag_kernel_base_address*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
    auto* mod_tag  = (stivale2_struct_tag_modules*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_MODULES_ID);
    assert(hhdm_tag != nullptr);
    assert(mmap_tag != nullptr);
    assert(pmr_tag != nullptr);
    assert(kbas_tag != nullptr);

    // Find the kernel's place in memory
    kernel::hhdm_set_vbase(hhdm_tag->addr);
    kernel::hhdm_set_kern_pbase(kbas_tag->physical_base_address);
    kernel::hhdm_set_kern_vbase(kbas_tag->virtual_base_address);
    kernel::g_pmm.init(); // Initialize the physical memory manager

    // Parse the memory map
    for (unsigned int i = 0; i < mmap_tag->entries; i++) {
        auto& map = mmap_tag->memmap[i];

        if (map.type == MMAP_TYPE_RAM) {
            g_pmm.add_region(kernel::mm::pmm::region(map.base, map.base + map.length, kernel::mm::pmm::region::region_type::ram));
            max_ram_addr = std::max(map.base + map.length, max_ram_addr);
        }
    }

    // Never use the null page, ever.
    g_pmm.mark_used(0);

    // Now, construct the kernel's page table
    pml_t* pml4 = (pml_t*)(hhdm_tag->addr + g_pmm.alloc_single(0));
    memset(pml4, sizeof(pml_t), 0);
    kernel_vas = vas((virt_addr_t)pml4, kernel::virt_to_phys_addr((virt_addr_t)pml4));

    // Map the HHDM
    size_t base = hhdm_tag->addr;
    size_t step = 0x200000;
    size_t max  = max_ram_addr;
    for (size_t i = 0; i < (max / step); i++) { kernel_vas.map((step * i), base + (step * i), 0x03, VAS_HUGE_PAGE); }

    // Map the kernel into the page table.
    uint64_t last_kernel_vaddr = 0;
    for (size_t i = 0; i < pmr_tag->entries; i++) {
        auto& pmr = pmr_tag->pmrs[i];

        for (size_t p = 0; p < pmr.length; p += PAGE_SIZE) {
            uint64_t offset = p + pmr.base;
            uint64_t paddr  = kbas_tag->physical_base_address + (offset - kbas_tag->virtual_base_address);

            uint32_t perm = 0x00;
            if (pmr.permissions && STIVALE2_PMR_READABLE) { perm |= 0x1; }
            if (pmr.permissions && STIVALE2_PMR_WRITABLE) { perm |= 0x2; }

            kernel_vas.map(paddr, offset, perm, 0);
            last_kernel_vaddr = std::max(last_kernel_vaddr, offset);
        }
    }

    // Map the Framebuffer and any modules, if needed.
    for (unsigned int i = 0; i < mmap_tag->entries; i++) {
        auto& map = mmap_tag->memmap[i];
        if (map.type == 0x1002) {
            for (size_t i = 0; i < map.length; i += PAGE_SIZE) { kernel_vas.map(map.base + i, (virt_addr_t)(con_fb.framebuffer.m_buffer) + i, 0x03, 0); }
        }

        // Map Modules
        if (map.type == 0x1001) {
            for (size_t j = 0; j < mod_tag->module_count; j++) {
                auto& mod = mod_tag->modules[j];

                if (kernel::phys_to_virt_addr(map.base) == mod.begin) {
                    kernel::boot::boot_file file;
                    if (strcmp("symbols", (const char*)&mod.string) == 0) {
                        file.type = 0x0;
                    } else {
                        file.type = 0x1;
                    }
                    file.size  = mod.end - mod.begin;
                    file.vaddr = mod.begin;
                    kernel::boot::g_bootfiles.add(file);

                    for (size_t k = 0; k < map.length; k += PAGE_SIZE) {
                        kernel_vas.map(map.base + k, mod.begin + k, 0x03, 0);
                        last_kernel_vaddr = std::max(last_kernel_vaddr, mod.begin + k);
                    }
                }
            }
        }
    }

    g_cpu_data[0].current_vas = &kernel_vas;
    set_page_table(g_cpu_data[0].current_vas->physical_addr());

    // VMM is now ready. Initialise the heap.
    g_heap.init(false, last_kernel_vaddr);
}
