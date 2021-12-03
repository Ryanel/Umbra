#include <kernel/critical.h>
#include <kernel/interrupts.h>
#include <kernel/scheduler.h>

void k_critical_enter() { kernel::scheduler::disable(); }

void k_critical_exit() { kernel::scheduler::enable(); }
