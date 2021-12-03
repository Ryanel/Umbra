#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/panic.h>
#include <kernel/scheduler.h>

void panic(const char* s) {
    kernel::scheduler::disable();
    interrupts_disable();

    kernel::log& log = kernel::log::get();
    kernel::log::critical("panic", "%s\n", s);
    log.flush();

    // TODO: Shut down all processors.
    while (true) {}
}
