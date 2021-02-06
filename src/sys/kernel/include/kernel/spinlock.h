#pragma once

#include <stdint.h>
#include <stdio.h>

namespace kernel {
namespace utils {
class spinlock {
   public:
    spinlock() {
        count = 0;
    }
    void release() {
        __atomic_clear(&count, __ATOMIC_RELEASE);
    }
    void acquire() {
        __atomic_test_and_set(&count, __ATOMIC_ACQUIRE);
    }

   public:
    __attribute__((aligned(8))) volatile uint64_t count = 0;
};
}  // namespace utils
}  // namespace kernel