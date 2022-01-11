#include <kernel/interrupts.h>
#include <kernel/log.h>

namespace kernel {

size_t             interrupts::page_faults_total;
size_t             interrupts::interrupts_total;
interrupt_handler* interrupts::handlers[256];

void interrupts::init() {
    page_faults_total = 0;
    interrupts_total  = 0;
}

void interrupts::handler_register(uintptr_t handler, interrupt_handler* hnd) { handlers[handler] = hnd; }
void interrupts::handler_unregister(uintptr_t handler) { handlers[handler] = 0; }

bool interrupts::dispatch(uintptr_t handler, register_frame_t* regs) {
    if (handlers[handler] != nullptr) { return handlers[handler]->handle_interrupt(regs); }
    kernel::log::warn("int", "Unhandled interrupt %d\n", handler);
    return false;
}

};  // namespace kernel