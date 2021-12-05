#pragma once

#include <kernel/interrupts.h>

namespace kernel {
class syscall_handler : public kernel::interrupt_handler {
    bool handle_interrupt(register_frame_t* regs);
};
}  // namespace kernel