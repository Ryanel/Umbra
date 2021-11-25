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
    uint64_t     time_elapsed;     // Time in NS this thread has been running
    int64_t      slice_ns;         // Time in NS for this threads slice
    uint32_t     id;               // The Thread ID of this thread
    thread_state state;            // Current state of this thread
};

class threading {
   public:
    static thread* create(task* owner, void* bootstrap_fn);
};

}  // namespace kernel

extern "C" void thread_switch(kernel::thread* to_switch);