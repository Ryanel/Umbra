#include <assert.h>
#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/panic.h>
#include <kernel/tasks/scheduler.h>

void panic(const char* s) {
    kernel::tasks::scheduler::lock();
    kernel::log::critical("panic", "%s\n", s);
    kernel::log::get().flush();

    // TODO: Shut down all processors.
    while (true) {
        asm("cli");
        asm("hlt");
    }
}

#ifdef KERNEL_ASSERT_ENABLED
void do_assert(bool expr, const source_location& loc, const char* expression) noexcept {
    if (!expr) {
        kernel::log::error("assert", "%s:%d, function %s : %s\n", loc.file_name, loc.line_number, loc.function_name,
                           expression);
        panic("Assertion failed");
    }
}
#endif