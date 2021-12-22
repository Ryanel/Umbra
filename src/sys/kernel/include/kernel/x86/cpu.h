#pragma once

#include <stdint.h>

namespace kernel {
struct cpu_data_t {
    uint32_t id;

    // x86 specific
    uint32_t lapic_id;
};
}  // namespace kernel