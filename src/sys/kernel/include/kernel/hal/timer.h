#pragma once

namespace kernel {
namespace hal {
namespace timer {
/// Represents a timer
class timer {
   public:
    int resolutionMS();  //< Resolution of the timer in milliseconds
    int resolutionUS();  //< Resolution of the in microseconds
};
}  // namespace time
}  // namespace hal
}  // namespace kernel