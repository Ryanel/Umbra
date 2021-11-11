#include <kernel/scheduler.h>

kernel::thread* current_tcb;

extern "C" uint32_t* stack_top;

kernel::thread                     idle_thread;
kernel::scheduler::scheduler_queue kernel::scheduler::ready_queue;

void kernel::scheduler::init() {
    current_tcb              = &idle_thread;
    current_tcb->k_stack_top = (uintptr_t)&stack_top;
    current_tcb->state       = thread_state::running;
    thread_switch(&idle_thread);
}

void kernel::scheduler::schedule() {
    if (!ready_queue.empty()) {
        thread* next = ready_queue.dequeue();
        ready_queue.enqueue(current_tcb); 
        thread_switch(next);
    }
}

void kernel::scheduler::enqueue_new(thread* t) { ready_queue.enqueue(t); }

extern "C" void sched_readd_thread(kernel::thread* t) { kernel::scheduler::ready_queue.enqueue(t); }