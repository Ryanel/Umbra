#pragma once

#include <kernel/hal/timer.h>
#include <stdint.h>

/// Represents a timer
class pit_timer : public kernel::hal::system_timer {
   public:
    void     init();                      //< Initialise
    void     set_phase(unsigned int hz);  //< Set the resolution
    void     tick();                      //< Will tick and increment the timer
    uint64_t resolution_ns();             //< Resolution in Nanoseconds
   private:
    unsigned int resolution_hz;
};
