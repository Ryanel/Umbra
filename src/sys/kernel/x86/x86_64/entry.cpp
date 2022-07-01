#include <assert.h>
#include <kernel/boot/boot_file.h>
#include <kernel/boot/stivale2.h>
#include <kernel/cpu.h>
#include <kernel/debug/symbol-file.h>
#include <kernel/hal/fb_text_console.h>
#include <kernel/hal/sw_framebuffer.h>
#include <kernel/hal/terminal.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/memory.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/time.h>
#include <kernel/x86/descriptor_table.h>
#include <kernel/x86/interrupts.h>
#include <kernel/x86/pager.h>
#include <kernel/x86/pit.h>
#include <kernel/x86/serial_text_console.h>
#include <kernel/x86/vas.h>
#include <kernel/x86/vga_text_console.h>
#include <stddef.h>
#include <stdint.h>

#include <algorithm>

using namespace kernel;

kernel::boot::boot_file_container   kernel::boot::g_bootfiles;
kernel::device::serial_text_console con_serial;
kernel::device::fb_text_console     con_fb;
kernel::device::vga_text_console    con_vga;
x86_idt                             g_idt;
pit_timer                           timer_pit;

void init_global_constructors();
void kernel_main();
void boot_init_memory(struct stivale2_struct* svs);

void kernel_print_version() { kernel::log::info("kernel", "Umbra v. %s on x86_64\n", KERNEL_VERSION); }

void boot_init_log(struct stivale2_struct* svs) {
    auto& log = kernel::log::get();
    log.init(&con_serial);
    log.setup();

    kernel::log::debug("kernel", "Kernel is alive!\n");

    // Setup graphics terminal
    auto* fb_info_tag     = (stivale2_struct_tag_framebuffer*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    auto* fb_textmode_tag = (stivale2_struct_tag_textmode*)stivale2_get_tag(svs, STIVALE2_STRUCT_TAG_TEXTMODE_ID);

    if (fb_info_tag != nullptr) {
        con_fb.framebuffer = sw_framebuffer((uint8_t*)fb_info_tag->framebuffer_addr, fb_info_tag->framebuffer_width, fb_info_tag->framebuffer_height,
                                            fb_info_tag->framebuffer_bpp, fb_info_tag->framebuffer_pitch, fb_format::bgr);
        log.init(&con_fb);

#ifdef LOG_ON_GRAPHICAL_CONSOLE
        log.m_terminal->set_output(&con_fb);
#endif
        kernel::log::debug("display", "Framebuffer: %dx%dx%d @ 0x%p from %s\n", fb_info_tag->framebuffer_width, fb_info_tag->framebuffer_height,
                           fb_info_tag->framebuffer_bpp, fb_info_tag->framebuffer_addr, &svs->bootloader_brand);
    } else {
        assert(fb_textmode_tag != nullptr);
        con_vga.buffer_address = fb_textmode_tag->address;
        log.init(&con_vga);
    }

    kernel::log::debug("kernel", "Log initialsing\n");
}

// The following will be our kernel's entry point.
extern "C" void _start(struct stivale2_struct* stivale2_struct) {
    init_global_constructors();
    boot_init_log(stivale2_struct);
    kernel_print_version();

    // Initialse each CPU into a ready-to-run state.
    size_t num_cores = 1;
    auto*  smp_tag   = (stivale2_struct_tag_smp*)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_SMP_ID);
    if (smp_tag != nullptr) {
        num_cores = smp_tag->cpu_count;
        kernel::log::info("smp", "%d cores detected\n", num_cores);
        for (size_t i = 0; i < num_cores; i++) {
            g_cpu_data[i].lapic_id = smp_tag->smp_info[i].lapic_id;
            g_cpu_data[i].id       = smp_tag->smp_info[i].processor_id;
            g_cpu_data[i].gdt.init(i);
        }
    }

    // Initialise the Boot Processor here...
    g_cpu_data[0].gdt.install();
    g_idt.init();

    // Initialise timer
    timer_pit.init();
    kernel::time::system_timer = &timer_pit;

    g_idt.enable_interrupts();  // Enable interrupts to fire. Primarily the page fault handler.

    // Initialise all modules...
    auto* mod_tag = (stivale2_struct_tag_modules*)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MODULES_ID);
    assert(mod_tag != nullptr);

    if (mod_tag->module_count <= 0) {
        log::critical("kernel", "An initial ramdisk must be loaded with the kernel. The kernel will not boot.\n");
        panic("No initrd loaded");
    }

    // Initialise the memory
    boot_init_memory(stivale2_struct);
    kernel::interrupts::handler_register(14, new kernel::x86_pager());  // Handle paging
    kernel::debug::g_symbol_server.init();                              // Initialise the symbol server, since we have the file mapped into memory now.

    // We've finished, so jump into the kernel main. If we return, halt.
    kernel_main();
    for (;;) { asm("hlt"); }
}
