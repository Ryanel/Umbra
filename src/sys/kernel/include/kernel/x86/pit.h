#pragma once

#include <kernel/hal/timer.h>
#include <kernel/interrupts.h>
#include <stdint.h>

/// Represents a timer
class pit_timer : public kernel::hal::system_timer, public kernel::interrupt_handler {
   public:
    void     init();                      //< Initialise
    void     set_phase(unsigned int hz);  //< Set the resolution
    void     tick();                      //< Will tick and increment the timer
    uint64_t resolution_ns();             //< Resolution in Nanoseconds
    bool     handle_interrupt(register_frame_t* regs);

   private:
    unsigned int resolution_hz;
};
