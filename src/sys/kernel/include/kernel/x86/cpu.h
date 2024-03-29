#pragma once

#include <kernel/x86/descriptor_table.h>
#include <kernel/x86/vas.h>
#include <stdint.h>

namespace kernel {
struct cpu_data_t {
    uint32_t id;           // The ID of this CPU.
    vas*     current_vas;  // Virtual address spaces
    x86::gdt gdt;          // The GDT for this core.
    // x86 specific
    uint32_t lapic_id;
};
}  // namespace kernel