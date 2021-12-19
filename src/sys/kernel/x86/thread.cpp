#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/panic.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/tasks/thread.h>

using namespace kernel::tasks;

extern "C" uint32_t* stack_top;

uint32_t next_thread_id = 0;

void thread_setup_function(void (*fn)(void)) {
    interrupts_enable();
    interrupts_after_thread();
    (*fn)();
    kernel::tasks::scheduler::terminate(nullptr);
}

void thread::setup(void* bootstrap_fn) {
    m_k_stack_top     = g_heap.alloc(0x1000, 0);
    m_k_stack_current = m_k_stack_top;
    m_id              = ++next_thread_id;

    if (m_owner == nullptr) { panic("Thread has no owner!"); }

    auto* stack_ptr   = (uintptr_t*)m_k_stack_current;
    *--stack_ptr      = 0;                                  // EAX
    *--stack_ptr      = 0;                                  // ECX
    *--stack_ptr      = (uintptr_t)bootstrap_fn;            // Return address as paramater
    *--stack_ptr      = 0;                                  // EDX
    *--stack_ptr      = (uintptr_t)&thread_setup_function;  // Address to call on thread start.
    *--stack_ptr      = 0;                                  // EBX
    *--stack_ptr      = 0;                                  // ESI
    *--stack_ptr      = 0;                                  // EDI
    *--stack_ptr      = 0;                                  // EBP
    m_k_stack_current = (uintptr_t)stack_ptr;
}
