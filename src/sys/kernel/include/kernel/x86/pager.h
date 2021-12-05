#pragma once

#include <kernel/interrupts.h>

namespace kernel {
class x86_pager : public kernel::interrupt_handler {
    bool handle_interrupt(register_frame_t* regs);
};

}  // namespace kernel