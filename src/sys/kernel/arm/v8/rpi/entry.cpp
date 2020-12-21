#include <kernel/arm/rpi/rpi.h>
#include <kernel/arm/rpi/rpi_uart0.h>
#include <kernel/delay.h>
#include <kernel/log.h>
#include <stdio.h>

void            kernel_main();
extern "C" void _start_aps();

int  numCoresOnline    = 1;
bool ap_early_finished = false;

/// The entry point of the boot core
extern "C" void _kernel_bp_start() {
    kernel::arm::rpi::rpi::get().init();            // Initialize Raspberry Pi specific code (and get version)
    kernel::device::rpi_uart_text_console console;  // Initialize UART0
    kernel::log::get().init(&console);              // Set UAR0 as the log output device

    printf("kernel: Waiting for cores to come online...\n");
    printf("kernel: Core 0 online\n");

    // BUG: Does not progress past this point on RPi due to kprintf spinlock bug!

    printf("kernel: Number of cores started on boot: %d\n", numCoresOnline);
    if (numCoresOnline <= 1) {
        printf("kernel: Bringing up additional cores...\n");
        _start_aps();
    }

    ap_early_finished = true;
    delay_cycles(1000);

    kernel_main();  // Enter the kernel..
}

extern "C" void _kernel_ap_start(uint64_t core) {
    numCoresOnline++;
    // Sasss
    while (!ap_early_finished) {}

    kprintf("kernel: Core %d online\n", core);

    while (true) { asm("wfi"); }
    return;
}

void kernel_print_version() {
    int version = kernel::arm::rpi::rpi::get().get_version();
    kprintf("kernel: Umbra v. 0.0.0.1 on armv8 (rpi)\n");
    kprintf("kernel: Detected a Raspberry pi %d\n", version);
}