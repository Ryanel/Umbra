#pragma once

#include <kernel/object.h>
#include <kernel/panic.h>
#include <kernel/tasks/thread.h>
#include <kernel/util/optional.h>
#include <stdint.h>

#include <list>

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
class task : public object {
   public:
    KOBJECT_CLASS_ID(1, "task");

    uintptr_t       m_vas;            // Pointer to the virutal address space of this process
    uint32_t        m_task_id;        // ID of this task.
    const char*     m_task_name;      // Name of this task
    vas*            m_directory;      // The directory
    handle_registry m_local_handles;  // All of this tasks handles

    std::list<task_file_descriptor> m_file_descriptors;
    uint32_t                        next_fd_id = 0;

    task(uintptr_t vas = 0, uint32_t id = 0, const char* name = nullptr)
        : m_vas(vas), m_task_id(id), m_task_name(name) {
        this->m_task_id   = id;
        this->m_task_name = name;
    }

    const char* name() { return m_task_name != nullptr ? m_task_name : "<unnamed task>"; }

    handle* spawn_local_thread(optional<const char*> name, void* bootstrap) {
        return m_local_handles.create(make_ref<thread>(new thread(this, bootstrap, name)), m_task_id, 0xFFFFFFFF, 1);
    }
};

}  // namespace tasks
}  // namespace kernel
