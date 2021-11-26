#include <kernel/critical.h>
#include <kernel/interrupts.h>
// TODO: Turn this into a semaphore.

volatile int critical_section_semaphore = 0;

void k_critical_enter() {
    interrupts_disable();
    critical_section_semaphore++;
}

void k_critical_exit() {
    critical_section_semaphore--;
    if (critical_section_semaphore == 0) { interrupts_enable(); }
    if (critical_section_semaphore < 0) { critical_section_semaphore = 0; }
}
