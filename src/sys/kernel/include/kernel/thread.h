#pragma once
#include <stdint.h>

namespace kernel {

enum class thread_state : uint8_t { dead, ready_to_run = 1, running = 2, blocked, sleeping };

struct thread {
    uintptr_t    k_stack_current;
    uintptr_t    k_stack_top;
    uint32_t     id;
    thread_state state;
    thread*      next;
};

class threading {
   public:
    static thread* create(void* starting_addr);
};

}  // namespace kernel

extern "C" void thread_switch(kernel::thread* to_switch);