#pragma once

#include <stdint.h>

class page_directory;

namespace kernel {
/// A task is a collection of resources, including at minimum a virtual address space.
struct task {
    uintptr_t   vas;        // Pointer to the virutal address space of this process
    uint32_t    task_id;    // ID of this task.
    const char* task_name;  // Name of this task
    page_directory * directory;

    task(uintptr_t vas = 0, uint32_t id = 0, const char* name = nullptr) : vas(vas), task_id(id), task_name(name) {
        if (vas == 0) { panic("Unimplemented: kernel::task::vas creation"); }

        this->task_id   = id;
        this->task_name = name;
    }

    const char* name() {
        if (task_name == nullptr) { return "<unnamed task>"; }
        return task_name;
    }
};
}  // namespace kernel
