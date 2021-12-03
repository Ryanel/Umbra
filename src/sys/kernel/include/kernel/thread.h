#pragma once

#include <kernel/types.h>
#include <stdint.h>

namespace kernel {

struct task;

enum class thread_state : uint8_t { dead, ready_to_run = 1, running = 2, blocked, sleeping };

/// A thread of execution
struct thread {
    virt_addr_t  m_k_stack_current;  // Current stack pointer
    virt_addr_t  m_k_stack_top;      // Stack top
    thread*      m_next;             // Next thread in whatever queue
    task*        m_owner;            // Owning task
    thread_state m_state;            // Current state of this thread
    uint64_t     m_time_elapsed;     // Time in NS this thread has been running
    uint64_t     m_slice_ns;         // Time in NS for this threads slice
    uint32_t     m_id;               // The Thread ID of this thread
    uint8_t      m_priority;         // Priority of this thread

    thread() { init_properties(); }

    thread(task* owner, void* bootstrap_fn) {
        init_properties();
        m_owner = owner;
        m_state = thread_state::ready_to_run;
        setup(bootstrap_fn);
    }

    void init_properties() {
        m_k_stack_current = 0;
        m_k_stack_top     = 0;
        m_next            = nullptr;
        m_owner           = nullptr;
        m_state           = thread_state::dead;
        m_time_elapsed    = 0;
        m_slice_ns        = 0;
        m_id              = 0;
        m_priority        = 128;
    }

    void setup(void* bootstrap_fn);

    bool ready() const { return m_state == thread_state::running || m_state == thread_state::ready_to_run; }
};

}  // namespace kernel

extern "C" void thread_switch(kernel::thread* to_switch);
