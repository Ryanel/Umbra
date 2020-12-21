#pragma once
#include <stdint.h>

namespace kernel {

/// Collection of utilities for using and manipulating ram.
class memory_utils {
   public:
    /// Spins until [address] & value == 0
    static inline void spin_flag_unset(uintptr_t address, uint32_t value) {
        do { asm volatile("nop"); } while (*((volatile unsigned int*)(address)) & value);
    }

    /// Spins until [address] & value != 0
    static inline void spin_flag_set(uintptr_t address, uint32_t value) {
        do { asm volatile("nop"); } while ((*((volatile unsigned int*)(address)) & value) == 0);
    }
};
}  // namespace kernel