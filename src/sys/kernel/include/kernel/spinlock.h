#pragma once

#include <stdint.h>
#include <stdio.h>

extern "C" void _k_spinlock_acquire(volatile uint32_t * var);

namespace kernel {
namespace utils {
class spinlock {
   public:
    spinlock() {
        count = 0;
    }
    void release() {
        count = 0;
    }
    void acquire() {
        //_k_spinlock_acquire(&count);
    }

   public:
    __attribute__((aligned(128))) volatile uint32_t count = 0;
};
}  // namespace utils
}  // namespace kernel