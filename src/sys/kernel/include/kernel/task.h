#pragma once

#include <stdint.h>

namespace kernel {

struct task {
    uintptr_t   vas;        // Pointer to the virutal address space of this process
    uint32_t    task_id;    // ID of this task.
    const char* task_name;  // Name of this task
};

}  // namespace kernel