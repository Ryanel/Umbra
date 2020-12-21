#include <kernel/log.h>
#include <kernel/panic.h>

void panic(const char* s) {
    // We use the kernel log directly instead of printf to reduce the dependancies needed.
    kernel::log& log = kernel::log::get();

    log.write("panic: ");
    log.write(s);
    log.write("\n");
    log.flush();

    // TODO: Shut down all processors.
    while (true) {}
}