#include <kernel/boot/multiboot.h>
#include <kernel/config.h>
#include <kernel/hal/fb_text_console.h>
#include <kernel/hal/sw_framebuffer.h>
#include <kernel/log.h>
#include <kernel/panic.h>
#include <kernel/time.h>
#include <kernel/x86/interrupts.h>
#include <kernel/x86/paging.h>
#include <kernel/x86/pit.h>
#include <kernel/x86/serial_text_console.h>
#include <kernel/x86/vga_text_console.h>
#include <string.h>

extern "C" void      _halt();
extern "C" uint32_t* boot_page_directory;

x86_idt                             g_idt;
kernel::device::vga_text_console    con_vga;
kernel::device::serial_text_console con_serial;
kernel::device::fb_text_console     con_fb;

void kernel_main();
void kernel_print_version() { klogf("kernel", "Umbra v. %s on x86 (i686)\n", KERNEL_VERSION); }

/// The responsibility of the kernel_entry function is to initialse the system into the minimuim startup state.
/// All architecture specific core functions (Tables, Paging, APs, Display) should be setup before control is transfered
/// to kernel_main
extern "C" void kernel_entry(uint32_t mb_magic, multiboot_info_t* mb_info) {
    paging_node paging_directory((page_directory_t*)(&boot_page_directory));

    // Initialise logging
    auto& log = kernel::log::get();
    con_serial.init();
    log.init(&con_serial);
    log.shouldBuffer = false;  // Disable buffering for now

    // Initialise hardware
    // Parse multiboot
    if (mb_magic != 0x2BADB002) {
        klogf("multiboot", "Multiboot magic was 0x%08x, halting!\n", mb_magic);
        panic("Multiboot magic incorrect");
    }

    // Initialise the display
    if (mb_info->framebuffer_type == 2) {
        klogf("display", "Using VGA 80x25 textmode\n");
        con_vga.init();
        log.init(&con_vga);
    } else {
        klogf("display", "Recieved framebuffer: %dx%dx%d @ 0x%p from multiboot\n", mb_info->framebuffer_width,
              mb_info->framebuffer_height, mb_info->framebuffer_bpp, mb_info->framebuffer_addr);

        con_fb.framebuffer = sw_framebuffer((uint8_t*)mb_info->framebuffer_addr, mb_info->framebuffer_width,
                                            mb_info->framebuffer_height, mb_info->framebuffer_bpp, mb_info->framebuffer_pitch);
        log.init(&con_fb);
        con_fb.init();
    }

    kernel_print_version();

    // Initialise the IDT
    g_idt.init();
    g_idt.enable_interrupts();

    // Initialise a timer
    pit_timer timer_pit;  // TODO: Fix this Hack! Should be dynamically allocated OR statically in Binary
    timer_pit.init();
    kernel::time::system_timer = &timer_pit;

    // Initialise the memory map (get it from GRUB)

    // Initialise paging

    // Call into the kernel now that all supported hardware is initialised.
    kernel_main();
}
