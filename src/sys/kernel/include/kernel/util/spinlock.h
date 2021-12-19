#pragma once

#include <stdint.h>
#include <stdio.h>

namespace kernel {
namespace util {
class spinlock {
   public:
    spinlock() { count = 0; }
    void release() { __atomic_clear(&count, __ATOMIC_RELEASE); }
    void acquire() {
        while (!__sync_bool_compare_and_swap(&count, 0, 1)) {}
    }

   public:
    __attribute__((aligned(8))) volatile uint64_t count = 0;
};
}  // namespace util
}  // namespace kernel