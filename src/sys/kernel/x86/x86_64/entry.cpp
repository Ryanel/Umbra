#include <assert.h>
#include <kernel/boot/boot_file.h>
#include <kernel/boot/stivale2.h>
#include <kernel/cpu.h>
#include <kernel/debug/symbol-file.h>
#include <kernel/hal/fb_text_console.h>
#include <kernel/hal/sw_framebuffer.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/memory.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/time.h>
#include <kernel/x86/pager.h>
#include <stddef.h>
#include <stdint.h>

#include <algorithm>
// x86
#include <kernel/x86/descriptor_table.h>
#include <kernel/x86/interrupts.h>
#include <kernel/x86/pit.h>
#include <kernel/x86/serial_text_console.h>
#include <kernel/x86/vas.h>
#include <kernel/x86/vga_text_console.h>

using namespace kernel;

// Stack in BSS
extern uint8_t stack[16384];

kernel::boot::boot_file_container   kernel::boot::g_bootfiles;
kernel::device::serial_text_console con_serial;
kernel::device::fb_text_console     con_fb;
kernel::device::vga_text_console    con_vga;
x86_idt                             g_idt;
pit_timer                           timer_pit;
vas                                 kernel_vas;

void            init_global_constructors();
extern "C" void set_page_table(uint64_t ptr);

static struct stivale2_header_tag_smp smp_hdr_tag = {
    // Same as above.
    .tag   = {.identifier = STIVALE2_HEADER_TAG_SMP_ID, .next = 0},
    .flags = 0,
};

static struct stivale2_header_tag_any_video tag_any_video = {
    // Same as above.
    .tag        = {.identifier = STIVALE2_HEADER_TAG_SMP_ID, .next = (uintptr_t)&smp_hdr_tag},
    .preference = 0,
};

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    // Same as above.
    .tag                = {.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, .next = (uintptr_t)&tag_any_video},
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0};

__attribute__((section(".stivale2hdr"), used)) static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack       = (uintptr_t)stack + sizeof(stack),
    .flags       = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags        = (uintptr_t)&framebuffer_hdr_tag};

void* stivale2_get_tag(struct stivale2_struct* stivale2_struct, uint64_t id) {
    struct stivale2_tag* current_tag = (stivale2_tag*)stivale2_struct->tags;
    for (;;) {
        if (current_tag == NULL) { return NULL; }
        if (current_tag->identifier == id) { return current_tag; }
        current_tag = (stivale2_tag*)current_tag->next;
    }
}

void kernel_print_version() { kernel::log::info("kernel", "Umbra v. %s on x86_64\n", KERNEL_VERSION); }

void boot_init_log(struct stivale2_struct* svs) {
    auto& log = kernel::log::get();
    log.init(&con_serial);
    log.shouldBuffer = false;  // Disable buffering for now

    kernel::log::debug("kernel", "Kernel is alive!\n");

    // Setup graphics terminal

    auto* fb_info_tag     = (stivale2_struct_tag_framebuffer*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    auto* fb_textmode_tag = (stivale2_struct_tag_textmode*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_TEXTMODE_ID);

    if (fb_info_tag != nullptr) {
        con_fb.framebuffer = sw_framebuffer((uint8_t*)fb_info_tag->framebuffer_addr, fb_info_tag->framebuffer_width,
                                            fb_info_tag->framebuffer_height, fb_info_tag->framebuffer_bpp,
                                            fb_info_tag->framebuffer_pitch, fb_format::bgr);
        log.init(&con_fb);

        kernel::log::debug("display", "Framebuffer: %dx%dx%d @ 0x%p from %s\n", fb_info_tag->framebuffer_width,
                           fb_info_tag->framebuffer_height, fb_info_tag->framebuffer_bpp, fb_info_tag->framebuffer_addr,
                           &svs->bootloader_brand);
    } else {
        assert(fb_textmode_tag != nullptr);
        con_vga.buffer_address = fb_textmode_tag->address;
        log.init(&con_vga);
    }

    kernel::log::debug("kernel", "Log initialsing");
    kernel::log::status_log("Done", 0xA);
}

void boot_init_memory(struct stivale2_struct* svs) {
    uint64_t max_ram_addr = 0;

    auto* hhdm_tag = (stivale2_struct_tag_hhdm*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_HHDM_ID);
    auto* mmap_tag = (stivale2_struct_tag_memmap*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    auto* pmr_tag  = (stivale2_struct_tag_pmrs*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_PMRS_ID);
    auto* kbas_tag =
        (stivale2_struct_tag_kernel_base_address*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
    auto* mod_tag = (stivale2_struct_tag_modules*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_MODULES_ID);
    assert(hhdm_tag != nullptr);
    assert(mmap_tag != nullptr);
    assert(pmr_tag != nullptr);
    assert(kbas_tag != nullptr);

    // Find the kernel's place in memory
    kernel::hhdm_set_vbase(hhdm_tag->addr);
    kernel::hhdm_set_kern_pbase(kbas_tag->physical_base_address);
    kernel::hhdm_set_kern_vbase(kbas_tag->virtual_base_address);

    // Start accounting physical memory.
    kernel::g_pmm.init();

    // Parse the memory map
    for (unsigned int i = 0; i < mmap_tag->entries; i++) {
        auto& map = mmap_tag->memmap[i];

        // Map only RAM
        if (map.type == 0x1) {
            g_pmm.add_region(
                kernel::mm::pmm::region(map.base, map.base + map.length, kernel::mm::pmm::region::region_type::ram));
            max_ram_addr = std::max(map.base + map.length, max_ram_addr);
        }
    }

    // Never use the null page, ever.
    g_pmm.mark_used(0x0000);

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

        for (size_t p = 0; p < pmr.length; p += 0x1000) {
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
            for (size_t i = 0; i < map.length; i += 0x1000) {
                kernel_vas.map(map.base + i, (virt_addr_t)(con_fb.framebuffer.m_buffer) + i, 0x03, 0);
            }
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

                    for (size_t k = 0; k < map.length; k += 0x1000) {
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

void kernel_main();

// The following will be our kernel's entry point.
extern "C" void _start(struct stivale2_struct* stivale2_struct) {
    init_global_constructors();
    boot_init_log(stivale2_struct);
    kernel_print_version();

    // Initialise basic hardware
    x86::g_gdt.init();  // Initialise the GDT
    g_idt.init();       // Initialise the IDT

    auto* smp_tag = (stivale2_struct_tag_smp*)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_SMP_ID);
    if (smp_tag != nullptr) {
        kernel::log::info("smp", "%d cores detected\n", smp_tag->cpu_count);
        for (size_t i = 0; i < smp_tag->cpu_count; i++) {
            g_cpu_data[i].lapic_id = smp_tag->smp_info[i].lapic_id;
            g_cpu_data[i].id       = smp_tag->smp_info[i].processor_id;
        }
    }

    // Interrupts
    timer_pit.init();
    kernel::time::system_timer = &timer_pit;
    g_idt.enable_interrupts();

    // Modules
    auto* mod_tag = (stivale2_struct_tag_modules*)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MODULES_ID);
    assert(mod_tag != nullptr);

    if (mod_tag->module_count <= 0) {
        log::critical("kernel", "An initial ramdisk must be loaded with the kernel. The kernel will not boot.\n");
        panic("No initrd loaded");
    }

    // Initialise the memory
    boot_init_memory(stivale2_struct);
    kernel::interrupts::handler_register(14, new kernel::x86_pager());  // Handle paging

    kernel::debug::g_symbol_server.init();

    // We're done, just hang...
    kernel_main();
    for (;;) { asm("hlt"); }
}
