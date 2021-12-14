#pragma once

#include <kernel/object.h>
#include <kernel/types.h>
#include <stdint.h>

#include <optional>

namespace kernel {
namespace tasks {

struct task;

enum class thread_state : uint8_t { dead, ready_to_run = 1, running = 2, blocked, sleeping };

/// A thread of execution
struct thread : public object {
    virt_addr_t                m_k_stack_current;  // Current stack pointer
    virt_addr_t                m_k_stack_top;      // Stack top
    task*                      m_owner;            // Owning task
    thread_state               m_state;            // Current state of this thread
    std::optional<const char*> m_name;             // Thread name
    uint64_t                   m_time_elapsed;     // Time in NS this thread has been running
    uint64_t                   m_slice_ns;         // Time in NS for this threads slice
    uint32_t                   m_id;               // The Thread ID of this thread
    int                        m_blocked;          // Block reason
    uint8_t                    m_priority;         // Priority of this thread

    KOBJECT_CLASS_ID(2, "thread");

    thread() { init_properties(); }

    thread(task* owner, void* bootstrap_fn, std::optional<const char*> name = std::nullopt) {
        init_properties();
        m_owner = owner;
        m_state = thread_state::ready_to_run;
        m_name  = name;
        setup(bootstrap_fn);
    }

    void init_properties() {
        m_k_stack_current = 0;
        m_k_stack_top     = 0;
        m_owner           = nullptr;
        m_state           = thread_state::dead;
        m_time_elapsed    = 0;
        m_slice_ns        = 0;
        m_id              = 0;
        m_priority        = 128;
    }

    void setup(void* bootstrap_fn);

    void set_state(thread_state state, uint64_t timeslice_left = 0) {
        m_state    = state;
        m_slice_ns = timeslice_left;
    }

    bool ready() const { return m_state == thread_state::running || m_state == thread_state::ready_to_run; }
};

}  // namespace tasks
}  // namespace kernel

extern "C" void thread_switch(kernel::tasks::thread* to_switch);
