#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/panic.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/tasks/thread.h>

using namespace kernel::tasks;

extern "C" uint32_t* stack_top;

uint32_t next_thread_id = 0;

void thread_setup_function(void (*fn)(uintptr_t arg0), uintptr_t arg0) {
    interrupts_enable();
    interrupts_after_thread();
    (*fn)(arg0);
    kernel::tasks::scheduler::terminate(nullptr);
}

void thread::setup(void* bootstrap_fn, uintptr_t arg0) {
    m_k_stack_top     = g_heap.alloc(PAGE_SIZE, 0);
    m_k_stack_current = m_k_stack_top;
    m_id              = ++next_thread_id;

    if (m_owner == nullptr) { panic("Thread has no owner!"); }

    auto* stack_ptr   = (uintptr_t*)m_k_stack_current;
    #ifdef ARCH_X86
    *--stack_ptr      = 0;                                  // EAX
    *--stack_ptr      = 0;                                  // ECX
    *--stack_ptr      = (uintptr_t)bootstrap_fn;            // Return address as paramater
    *--stack_ptr      = 0;                                  // EDX
    *--stack_ptr      = (uintptr_t)&thread_setup_function;  // Address to call on thread start.
    *--stack_ptr      = 0;                                  // EBX
    *--stack_ptr      = 0;                                  // ESI
    *--stack_ptr      = 0;                                  // EDI
    *--stack_ptr      = 0;                                  // EBP
    #else
    *--stack_ptr      = (uintptr_t)&thread_setup_function;  // Address to call on thread start.
    *--stack_ptr      = 0;                                  // RAX
    *--stack_ptr      = 0;                                  // RCX
    *--stack_ptr      = 0;                                  // RDX
    *--stack_ptr      = 0;                                  // RBX
    *--stack_ptr      = 0;                                  // RBP
    *--stack_ptr      = arg0;                               // RSI
    *--stack_ptr      = (uintptr_t)bootstrap_fn;            // RDI (Return address as param to setup function)
    *--stack_ptr      = 0;                                  // R8
    *--stack_ptr      = 0;                                  // R9
    *--stack_ptr      = 0;                                  // R10
    *--stack_ptr      = 0;                                  // R11
    *--stack_ptr      = 0;                                  // R12
    *--stack_ptr      = 0;                                  // R13
    *--stack_ptr      = 0;                                  // R14
    *--stack_ptr      = 0;                                  // R15
    #endif
    m_k_stack_current = (uintptr_t)stack_ptr;
}
