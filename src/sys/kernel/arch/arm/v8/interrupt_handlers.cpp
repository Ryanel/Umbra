#include <kernel/log.h>
#include <stdio.h>

extern "C" void kernel_interrupt_exception() {
    printf("OOPS :)\n");
    while(true) {}
}

extern "C" void kernel_interrupt_irq() {
    printf("IRQ!\n");
    while(true) {}
}