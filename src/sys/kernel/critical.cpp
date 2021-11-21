#include <kernel/critical.h>
#include <kernel/interrupts.h>
// TODO: Turn this into a semaphore.

void k_critical_enter() { interrupts_disable(); }
void k_critical_exit() { interrupts_enable(); }