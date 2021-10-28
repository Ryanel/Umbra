#include <kernel/boot/multiboot.h>
#include <kernel/config.h>
#include <kernel/hal/fb_text_console.h>
#include <kernel/hal/sw_framebuffer.h>
#include <kernel/log.h>
#include <kernel/panic.h>
#include <kernel/x86/interrupts.h>
#include <kernel/x86/serial_text_console.h>
#include <kernel/x86/vga_text_console.h>
#include <string.h>

extern "C" void _halt();

x86_idt g_idt;

kernel::device::vga_text_console    text_console;
kernel::device::serial_text_console serial_console;

void kernel_main();

void kernel_print_version() { klogf("kernel", "Umbra v. %s on x86 (i686)\n", KERNEL_VERSION); }

/// The responsibility of the kernel_entry function is to initialse the system into the minimuim startup state.
/// All architecture specific core functions (Tables, Paging, APs, Display) should be setup before control is transfered
/// to kernel_main
extern "C" void kernel_entry(uint32_t multiboot_magic, multiboot_info_t* multiboot_header) {
    // Initialise logging

    auto& log = kernel::log::get();

    serial_console.init();
    log.init(&serial_console);

    log.shouldBuffer = false;  // Disable buffering for now

    // Initialise hardware
    // Parse multiboot
    if (multiboot_magic != 0x2BADB002) {
        klogf("multiboot", "Multiboot magic was %p, halting!\n", multiboot_magic);
        panic("Multiboot magic incorrect");
    }

    if (multiboot_header->framebuffer_type == 2) {
        klogf("display", "Using VGA 80x25 textmode\n");
        text_console.init();
        log.init(&text_console);
    } else {
        klogf("display", "Recieved framebuffer: %dx%dx%d @ 0x%p from multiboot\n", multiboot_header->framebuffer_width,
              multiboot_header->framebuffer_height, multiboot_header->framebuffer_bpp, multiboot_header->framebuffer_addr);
        kernel::device::fb_text_console fb_console;
        fb_console.framebuffer.width  = multiboot_header->framebuffer_width;
        fb_console.framebuffer.height = multiboot_header->framebuffer_height;
        fb_console.framebuffer.bpp    = multiboot_header->framebuffer_bpp;
        fb_console.framebuffer.pitch  = multiboot_header->framebuffer_pitch;
        fb_console.framebuffer.buffer = (uint8_t*)multiboot_header->framebuffer_addr;
        log.init(&fb_console);
        fb_console.init();
    }

    kernel_print_version();

    // Initialise the IDT
    g_idt.init();
    // g_idt.enable_interrupts();

    // TODO: Initialise TSS

    // TODO: Initialise the memory map (get it from GRUB)

    // TODO: Initialise paging (do this before kernel_entry!)

    // TODO: Initialise PIT

    // TODO: Initialsie VGA display output

    // Call into the kernel now that all supported hardware is initialised.
    kernel_main();
}
