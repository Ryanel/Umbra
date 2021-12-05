#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/panic.h>
#include <kernel/scheduler.h>

void panic(const char* s) {
    kernel::scheduler::lock();
    kernel::log::critical("panic", "%s\n", s);
    kernel::log::get().flush();

    // TODO: Shut down all processors.
    while (true) {
        asm("cli; hlt");
    }
}
