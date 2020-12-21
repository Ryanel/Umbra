#include <kernel/arm/uart_text_console.h>
#include <kernel/config.h>
#include <kernel/log.h>
#include <stdint.h>
#include <stdio.h>
void kernel_main();

extern "C" void kernel_entry() {
    kernel::device::uart_text_console console;
    console.wait_for_ldr = false;
    console.set_address(0x1c090000);

    auto& log = kernel::log::get();
    log.init(&console);

    log.write("kernel: Umbra - ARM v7a Cortex A15\n");

    printf("AAA");
    log.flush();

    kernel_main();

    while (true) {}
}