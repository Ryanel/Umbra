#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/panic.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>

extern "C" uint32_t* stack_top;

uint32_t next_thread_id = 0;

void thread_setup_function(void (*fn)(void)) {
    interrupts_enable();
    interrupts_after_thread();
    (*fn)();
    kernel::scheduler::terminate(nullptr);
}

void kernel::thread::setup(void* bootstrap_fn) {
    k_stack_top     = g_heap.alloc(0x1000, KHEAP_PAGEALIGN);
    k_stack_current = k_stack_top;
    id              = ++next_thread_id;

    if (owner == nullptr) { panic("Thread has no owner!"); }

    auto* stack_ptr    = (uint32_t*)k_stack_current;
    *--stack_ptr       = 0;                                 // EAX
    *--stack_ptr       = 0;                                 // ECX
    *--stack_ptr       = (uint32_t)bootstrap_fn;            // Return address as paramater
    *--stack_ptr       = 0;                                 // EDX
    *--stack_ptr       = (uint32_t)&thread_setup_function;  // Address to call on thread start.
    *--stack_ptr       = 0;                                 // EBX
    *--stack_ptr       = 0;                                 // ESI
    *--stack_ptr       = 0;                                 // EDI
    *--stack_ptr       = 0;                                 // EBP
    k_stack_current = (uint32_t)stack_ptr;
}
