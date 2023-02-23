#include <assert.h>
#include <kernel/debug/symbol-file.h>
#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/panic.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/cpu.h>
#include <kernel/x86/cpu.h>

extern kernel::stackframe interrupt_stackframe;

static inline void print_stack_trace( kernel::stackframe* stk) {
    constexpr int maxFrames = 20;
    for (int frame = 0; stk && frame < maxFrames; frame++) {
        if (stk->pc == 0) { break; }
        const char* symname = kernel::debug::g_symbol_server.get_symbol(stk->pc);
        kernel::log::critical("* ", "0x%016p ( %s )\n", stk->pc, symname);
        stk = stk->rbp;
    }
}

void panic(const char* s) {
    kernel::tasks::scheduler::lock();
    kernel::log::critical("panic", "%s\n", s);    

    // If there's an exception
    if (interrupt_stackframe.rbp != 0) {
        kernel::log::critical("panic", "Stack trace before exception:\n");
        print_stack_trace(&interrupt_stackframe);
    }
    else {
        kernel::log::critical("panic", "Stack Trace:\n");
        interrupt_stackframe = kernel::cpu_get_stackframe();
        print_stack_trace(&interrupt_stackframe);
    }

    // TODO: Shut down all processors.
    while (true) {
        asm("cli");
        asm("hlt");
    }
}

#ifdef KERNEL_ASSERT_ENABLED
void do_assert(bool expr, const source_location& loc, const char* expression) noexcept {
    if (!expr) {
        kernel::log::critical("assert", "Assertion failed.\n");
        kernel::log::critical("assert", "%s, %s:%d => %s\n", loc.file_name, loc.function_name, loc.line_number,
                              expression);
        panic("Assertion failed");
    }
}
#endif