#pragma once

#include <kernel/regs.h>
#include <kernel/types.h>

void interrupts_disable();
void interrupts_enable();
void interrupts_after_thread();

namespace kernel {

class interrupt_handler {
   public:
    virtual bool handle_interrupt(register_frame_t* regs) = 0;
};

class interrupts {
   public:
    static void init();
    static void handler_register(uintptr_t handler, interrupt_handler* hnd);
    static void handler_unregister(uintptr_t handler);
    static bool dispatch(uintptr_t handler, register_frame_t* regs);

   private:
    static size_t             page_faults_total;
    static size_t             interrupts_total;
    static interrupt_handler* handlers[256];
};
};  // namespace kernel