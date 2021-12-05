#pragma once

#include <stdint.h>

namespace kernel {

class vas;

/// A task is a collection of resources, including at minimum a virtual address space.
struct task {
    uintptr_t       m_vas;        // Pointer to the virutal address space of this process
    uint32_t        m_task_id;    // ID of this task.
    const char*     m_task_name;  // Name of this task
    vas* m_directory;  // The directory

    task(uintptr_t vas = 0, uint32_t id = 0, const char* name = nullptr) : m_vas(vas), m_task_id(id), m_task_name(name) {
        if (vas == 0) { panic("Unimplemented: kernel::task::vas creation"); }

        this->m_task_id   = id;
        this->m_task_name = name;
    }

    const char* name() {
        if (m_task_name == nullptr) { return "<unnamed task>"; }
        return m_task_name;
    }
};
}  // namespace kernel
