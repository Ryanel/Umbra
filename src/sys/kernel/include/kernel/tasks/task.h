#pragma once

#include <kernel/util/linked_list.h>
#include <stdint.h>

namespace kernel {

class vas;

namespace vfs {
struct file_descriptor;
}

struct task_file_descriptor {
    uint32_t              m_local_id;
    vfs::file_descriptor* m_descriptor;
};

namespace tasks {

/// A task is a collection of resources, including at minimum a virtual address space.
struct task {
    uintptr_t   m_vas;        // Pointer to the virutal address space of this process
    uint32_t    m_task_id;    // ID of this task.
    const char* m_task_name;  // Name of this task
    vas*        m_directory;  // The directory

    util::linked_list<task_file_descriptor> m_file_descriptors;
    uint32_t                                next_fd_id = 0;

    task(uintptr_t vas = 0, uint32_t id = 0, const char* name = nullptr)
        : m_vas(vas), m_task_id(id), m_task_name(name) {
        if (vas == 0) { panic("Unimplemented: kernel::task::vas creation"); }

        this->m_task_id   = id;
        this->m_task_name = name;
    }

    const char* name() {
        if (m_task_name == nullptr) { return "<unnamed task>"; }
        return m_task_name;
    }
};

}  // namespace tasks
}  // namespace kernel
