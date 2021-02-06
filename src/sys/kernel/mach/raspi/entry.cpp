#include <kernel/arm/rpi/rpi.h>
#include <kernel/arm/rpi/rpi_uart0.h>
#include <kernel/delay.h>
#include <kernel/log.h>
#include <stdio.h>

void            kernel_main();
extern "C" void _start_aps();
extern "C" void kernel_interrupt_enable();
extern "C" void kernel_interrupt_init();

volatile int numCoresOnline = 0;

/// The entry point of the boot core
extern "C" void _kernel_bp_start() {
    kernel::arm::rpi::rpi::get().init();            // Initialize Raspberry Pi specific code (and get version)
    kernel::device::rpi_uart_text_console console;  // Initialize UART0
    kernel::log::get().init(&console);              // Set UAR0 as the log output device

    kprintf("Waiting for cores to come online...\n");
    kprintf("Core 0 online\n");

    if (numCoresOnline <= 4) {
        kprintf("Bringing up additional cores...\n");
        _start_aps();
    }


    kprintf("Enabling interrupts\n");
    kernel_interrupt_init();
    kernel_interrupt_enable();
    asm("svc 0"); 
    kernel_main();  // Enter the kernel..
}

extern "C" void _kernel_ap_start(uint64_t core) {
    numCoresOnline++;
    kprintf("Core %d online\n", core);

    while (true) { 
        asm("wfi"); 
    }
}

void kernel_print_version() {
    int version = kernel::arm::rpi::rpi::get().get_version();
    kprintf("kernel: Umbra v. 0.0.0.1 on armv8 (rpi)\n");
    kprintf("kernel: Detected a Raspberry pi %d\n", version);
}