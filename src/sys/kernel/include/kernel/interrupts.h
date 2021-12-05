#pragma once

#include <kernel/regs.h>

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
    static void handler_register(int handler, interrupt_handler* hnd);
    static void handler_unregister(int handler);
    static bool dispatch(int handler, register_frame_t* regs);

   private:
    static int                page_faults_total;
    static int                interrupts_total;
    static interrupt_handler* handlers[256];
};
};  // namespace kernel