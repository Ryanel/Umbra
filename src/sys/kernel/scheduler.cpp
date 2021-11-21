#include <kernel/log.h>
#include <kernel/scheduler.h>
#include <kernel/interrupts.h>

using namespace kernel;

kernel::thread*      current_tcb;
task*                current_task;
extern "C" uint32_t* stack_top;

thread idle_thread;
task   _kernel_task;
task*  scheduler::kernel_task;

scheduler::scheduler_queue scheduler::ready_queue;

void scheduler::init(phys_addr_t kernel_vas) {
    _kernel_task.task_id   = 0;
    _kernel_task.task_name = "[kernel idle]";
    _kernel_task.vas       = kernel_vas;
    current_task           = &_kernel_task;
    scheduler::kernel_task = &_kernel_task;

    current_tcb              = &idle_thread;
    current_tcb->k_stack_top = (uintptr_t)&stack_top;
    current_tcb->state       = thread_state::running;
    current_tcb->id          = 0;
    current_tcb->owner       = kernel_task;

    interrupts_disable();
    thread_switch(&idle_thread);
    interrupts_enable();
}

void scheduler::schedule() {
    debug();
    if (!ready_queue.empty()) {
        auto* next = ready_queue.dequeue();
        ready_queue.enqueue(current_tcb);

        interrupts_disable();
        thread_switch(next);
        interrupts_enable();
    }
}

void scheduler::debug() {
    klogf("sched", "%d rtr, cur %d (%s)/tid %d\n", ready_queue.size(), current_task->task_id, current_task->task_name,
          current_tcb->id);
}

void            scheduler::enqueue_new(thread* t) { ready_queue.enqueue(t); }
void            scheduler::enqueue(thread* t) { ready_queue.enqueue(t); }
extern "C" void sched_readd_thread(thread* t) { scheduler::enqueue(t); }
