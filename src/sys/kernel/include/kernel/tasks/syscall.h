#pragma once

#include <kernel/interrupts.h>

namespace kernel {
namespace tasks {

class syscall_handler : public kernel::interrupt_handler {
    bool handle_interrupt(register_frame_t* regs);
};

}  // namespace tasks
}  // namespace kernel