#include <kernel/interrupts.h>
#include <kernel/log.h>

namespace kernel {

int                interrupts::page_faults_total;
int                interrupts::interrupts_total;
interrupt_handler* interrupts::handlers[256];

void interrupts::init() {
    page_faults_total = 0;
    interrupts_total  = 0;
}

void interrupts::handler_register(int handler, interrupt_handler* hnd) { handlers[handler] = hnd; }
void interrupts::handler_unregister(int handler) { handlers[handler] = 0; }

bool interrupts::dispatch(int handler, register_frame_t* regs) {
    if (handlers[handler] != nullptr) { return handlers[handler]->handle_interrupt(regs); }
    kernel::log::warn("int", "Unhandled interrupt %d\n", regs->int_no);
    return false;
}

};  // namespace kernel