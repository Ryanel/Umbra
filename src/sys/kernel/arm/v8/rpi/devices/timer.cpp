#include <kernel/arm/rpi/rpi.h>
#include <kernel/arm/rpi/rpi_uart0.h>
#include <kernel/delay.h>

void delay_cycles(uint32_t cycles) {
    while (cycles--) { asm volatile("nop"); }
}
