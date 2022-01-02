#include <assert.h>
#include <kernel/debug/symbol-file.h>
#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/panic.h>
#include <kernel/tasks/scheduler.h>

struct stackframe {
    struct stackframe* rbp;
    uintptr_t          pc;
};

extern uint64_t interrupt_stack_ptr;

void panic(const char* s) {
    const int maxFrames = 5;
    kernel::tasks::scheduler::lock();
    kernel::log::critical("panic", "%s\n", s);
    kernel::log::critical("panic", "Stack Trace:\n");


    {
        struct stackframe* stk = (stackframe*)__builtin_frame_address(0);
        for (int frame = 0; stk && frame < maxFrames; frame++) {
            if (stk->pc == 0) { break; }
            const char* symname = kernel::debug::g_symbol_server.get_symbol(stk->pc);
            kernel::log::critical("trace", "0x%016p ( %s )\n", stk->pc, symname);
            stk = stk->rbp;
        }
    }

    {
        // If there's an exception
        if (interrupt_stack_ptr != 0) {
            kernel::log::critical("panic", "Before Exception Trace:\n");
            struct stackframe* stk = (stackframe*)interrupt_stack_ptr;
            for (int frame = 0; stk && frame < maxFrames; frame++) {
                if (stk->pc == 0) { break; }
                const char* symname = kernel::debug::g_symbol_server.get_symbol(stk->pc);
                kernel::log::critical("trace", "0x%016p ( %s )\n", stk->pc, symname);
                stk = stk->rbp;
            }
        }
    }

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
        kernel::log::critical("assert", "Assertion failed.\n");
        kernel::log::critical("assert", "%s, %s:%d => %s\n", loc.file_name, loc.function_name, loc.line_number,
                              expression);
        panic("Assertion failed");
    }
}
#endif