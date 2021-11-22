#include <kernel/critical.h>
#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/scheduler.h>
#include <stdio.h>

using namespace kernel;

kernel::thread*      current_tcb;
task*                current_task;
extern "C" uint32_t* stack_top;

thread idle_thread;
task   _kernel_task;

task*                      scheduler::kernel_task;
int                        scheduler::scheduler_lock = 0;
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

    ready_queue.state = thread_state::ready_to_run;

    critical_section cs;
    thread_switch(&idle_thread);
}

void scheduler::schedule() {
    if (scheduler_lock != 0) { return; }

    critical_section cs;

    if (!ready_queue.empty()) {
        debug();
        auto* next = ready_queue.dequeue();

        // Only enqueue processes that are running.
        if (current_tcb->state == thread_state::running || current_tcb->state == thread_state::ready_to_run) { ready_queue.enqueue(current_tcb); }
        thread_switch(next);
    }
}

const char* thread_state_to_name(thread_state state) {
    switch (state) {
        case thread_state::running:
            return "run";
        case thread_state::ready_to_run:
            return "ready";
        case thread_state::dead:
            return "dead";
        case thread_state::blocked:
            return "block";
        case thread_state::sleeping:
            return "sleep";
        default:
            return "?????";
    }
}

void scheduler::debug() {
    lock();

    printf("+-TID-+-TASK NAME------------------------------------+-STATE-+\n");

    printf("| %3d | %3d/%-40s | %5s |\n", current_tcb->id, current_tcb->owner->task_id, current_tcb->owner->task_name,
           thread_state_to_name(current_tcb->state));
    for (thread* t = ready_queue.top(); t != nullptr; t = t->next) {
        printf("| %3d | %3d/%-40s | %5s |\n", t->id, t->owner->task_id, t->owner->task_name, thread_state_to_name(t->state));
    }

    printf("+-----+----------------------------------------------+-------+\n");

    unlock();
}

void scheduler::enqueue_new(thread* t) { enqueue(t); }

void scheduler::enqueue(thread* t) {
    critical_section cs;
    ready_queue.enqueue(t);
}

void scheduler::terminate(thread* t) {
    if (t == nullptr) { t = current_tcb; }
    t->state = thread_state::dead;
    schedule();
}

void scheduler::lock() { scheduler_lock++; }
void scheduler::unlock() { scheduler_lock--; }

extern "C" void sched_readd_thread(thread* t) { scheduler::enqueue(t); }
