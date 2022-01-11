#pragma once

#include <kernel/hal/keyboard.h>
#include <kernel/interrupts.h>

namespace kernel {
namespace driver {

class ps2keyboard : public interrupt_handler, public hal::keyboard {
   public:
    ps2keyboard() { init(); }
    void init();
    bool handle_interrupt(register_frame_t* regs);

   private:
    const uint16_t data_port   = 0x60;
    const uint16_t status_reg  = 0x64;
    const uint16_t command_reg = 0x64;

    bool extended_e0 = false;  // Last scancode was E0

    char scancode_queue[256];
};

}  // namespace driver
}  // namespace kernel