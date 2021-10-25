#include <kernel/log.h>
#include <kernel/x86/vga_text_console.h>
#include <string.h>
#include <kernel/x86/interrupts.h>
#include <kernel/config.h>

extern "C" void _halt();

x86_idt g_idt;

void kernel_main();

void kernel_print_version() {
    kprintf("kernel: Umbra v. %s on x86 (i686)\n", KERNEL_VERSION);
}

/// The responsibility of the kernel_entry function is to initialse the system into the minimuim startup state.
/// All architecture specific core functions (Tables, Paging, APs, Display) should be setup before control is transfered
/// to kernel_main
extern "C" void kernel_entry() {
    // Initialise logging
    kernel::device::vga_text_console console;
    console.init();

    auto& log = kernel::log::get();
    log.init(&console);
    log.shouldBuffer = false;  // Disable buffering for now
    kernel_print_version();

    // Initialise hardware

    // Could be moved to loader...
    // TODO: Call global constructors
    // TODO: Initialise the IDT
    g_idt.init();
    g_idt.enable_interrupts();
    // TODO: Initialise TSS
    // TODO: Initialise paging (do this before kernel_entry!)
    // TODO: Initialise PIT
    // TODO: Initialsie VGA display output
    // TODO: Initialise the memory map (get it from GRUB)

    // Call into the kernel now that all supported hardware is initialised.
    kernel_main();
}


