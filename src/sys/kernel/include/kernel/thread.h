#pragma once
#include <kernel/task.h>
#include <kernel/types.h>
#include <stdint.h>

namespace kernel {

enum class thread_state : uint8_t { dead, ready_to_run = 1, running = 2, blocked, sleeping };

/// A thread of execution
struct thread {
    virt_addr_t  k_stack_current;  // Current stack pointer
    virt_addr_t  k_stack_top;      // Stack top
    thread*      next;             // Next thread in whatever queue
    task*        owner;            // Owning task
    thread_state state;            // Current state of this thread
    uint64_t     time_elapsed;     // Time in NS this thread has been running
    uint64_t     slice_ns;         // Time in NS for this threads slice
    uint32_t     id;               // The Thread ID of this thread
    uint8_t      priority;         // Priority of this thread

    thread() { init_properties(); }

    thread(task* owner, void* bootstrap_fn) {
        init_properties();
        this->owner = owner;
        this->state = thread_state::ready_to_run;
        setup(bootstrap_fn);
    }

    void init_properties() {
        k_stack_current = 0;
        k_stack_top     = 0;
        next            = nullptr;
        owner           = nullptr;
        state           = thread_state::dead;
        time_elapsed    = 0;
        slice_ns        = 0;
        id              = 0;
        priority        = 128;
    }

    void setup(void* bootstrap_fn);
};

}  // namespace kernel

extern "C" void thread_switch(kernel::thread* to_switch);