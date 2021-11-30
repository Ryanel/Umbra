#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/panic.h>

void panic(const char* s) {
    interrupts_disable();
    // We use the kernel log directly instead of printf to reduce the dependancies needed.
    kernel::log& log = kernel::log::get();
    kernel::log::critical("panic", "%s\n", s);
    log.flush();

    // TODO: Shut down all processors.
    while (true) {}
}
