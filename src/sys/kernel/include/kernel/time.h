#pragma once

#include <kernel/hal/timer.h>
#include <stdint.h>

namespace kernel {
class time {
   public:
    static void     increment(uint64_t ns);
    static uint64_t boot_time_ns();
    static uint64_t time_ns();

   public:
    static hal::system_timer* system_timer;

   private:
    static uint64_t time_since_boot;
    static uint64_t real_time;
};
}  // namespace kernel