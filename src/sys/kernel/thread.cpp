#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/thread.h>
extern "C" uint32_t* stack_top;

uint32_t next_thread_id = 0;

void thread_setup_function(void (*fn)(void)) {
    klogf("thread", "Created new thread!\n");
    (*fn)();

    // If a thread ever quits, destory it here automatically
    klogf("thread", "Destroying thread!\n");
    while (true) { /* code */
    }
}

kernel::thread* kernel::threading::create(void* fn) {
    auto* t            = new thread();
    t->k_stack_top     = g_heap.alloc(0x1000, KHEAP_PAGEALIGN);
    t->k_stack_current = t->k_stack_top;
    t->state           = thread_state::ready_to_run;
    t->id              = ++next_thread_id;

    auto* stack_ptr = (uint32_t*)t->k_stack_current;
    *--stack_ptr    = 0;                                 // EAX
    *--stack_ptr    = 0;                                 // ECX
    *--stack_ptr    = (uint32_t)fn;                      // Return address as paramater
    *--stack_ptr    = 0;                                 // EDX
    *--stack_ptr    = (uint32_t)&thread_setup_function;  // Address to call on thread start.
    *--stack_ptr    = 0;                                 // EBX
    *--stack_ptr    = 0;                                 // ESI
    *--stack_ptr    = 0;                                 // EDI
    *--stack_ptr    = 0;                                 // EBP

    t->k_stack_current = (uint32_t)stack_ptr;

    return t;
}
