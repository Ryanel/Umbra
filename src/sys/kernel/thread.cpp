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

kernel::thread* kernel::threading::create(task* owner, void* bootstrap_fn) {
    auto* t            = new thread();
    t->k_stack_top     = g_heap.alloc(0x1000, KHEAP_PAGEALIGN);
    t->k_stack_current = t->k_stack_top;
    t->state           = thread_state::ready_to_run;
    t->id              = ++next_thread_id;
    t->next            = nullptr;
    t->owner           = owner;
    t->slice_ns        = 0;

    if (t->owner == nullptr) { panic("Thread has no owner!"); }

    auto* stack_ptr    = (uint32_t*)t->k_stack_current;
    *--stack_ptr       = 0;                                 // EAX
    *--stack_ptr       = 0;                                 // ECX
    *--stack_ptr       = (uint32_t)bootstrap_fn;            // Return address as paramater
    *--stack_ptr       = 0;                                 // EDX
    *--stack_ptr       = (uint32_t)&thread_setup_function;  // Address to call on thread start.
    *--stack_ptr       = 0;                                 // EBX
    *--stack_ptr       = 0;                                 // ESI
    *--stack_ptr       = 0;                                 // EDI
    *--stack_ptr       = 0;                                 // EBP
    t->k_stack_current = (uint32_t)stack_ptr;
    return t;
}
