#pragma once

#include <stdint.h>

namespace kernel {
namespace hal {

/// Represents a timer
class system_timer {
   public:
    virtual void     init()          = 0;  //< Initialise
    virtual void     tick()          = 0;  //< Will tick and increment the timer
    virtual uint64_t resolution_ns() = 0;  //< Resolution in Nanoseconds
   public:
    uint64_t ticks;
};

}  // namespace hal
}  // namespace kernel