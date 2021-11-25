#include <kernel/critical.h>
#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/scheduler.h>
#include <kernel/time.h>
#include <stdio.h>

using namespace kernel;

kernel::thread*      current_tcb;
task*                current_task;
extern "C" uint32_t* stack_top;

thread idle_thread;
task   _kernel_task;

task*                      scheduler::kernel_task;
int                        scheduler::scheduler_lock   = 0;
uint64_t                   scheduler::last_schedule_ns = 0;
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
    last_schedule_ns  = 0;

    critical_section cs;
    thread_switch(&idle_thread);
}

void scheduler::schedule() {
    critical_section cs;

    const bool allow_swap_process = scheduler_lock == 0;
    bool       state_should_swap  = false;

    // Update slice accounting
    int64_t elapsed = (int64_t)(kernel::time::boot_time_ns() - last_schedule_ns);
    current_tcb->slice_ns -= elapsed;
    current_tcb->time_elapsed += elapsed;

    last_schedule_ns = kernel::time::boot_time_ns();

    // Should we swap?
    if (current_tcb->slice_ns <= 0) { state_should_swap = true; }

    if (state_should_swap && allow_swap_process) {
        if (!ready_queue.empty()) {
            debug();
            auto* next = ready_queue.dequeue();

            next->slice_ns        = 25000000;
            current_tcb->slice_ns = 0;

            // Only enqueue processes that are running.
            if (current_tcb->state == thread_state::running || current_tcb->state == thread_state::ready_to_run) {
                ready_queue.enqueue(current_tcb);
            }
            thread_switch(next);
        }
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

    printf("+-Tid-+-Task Name--------------------------+-Time-(s)+-State-+\n");

    uint64_t cms = current_tcb->time_elapsed / (uint64_t)1000000;
    uint32_t csecs      = cms / 1000;
    uint32_t chundreths = cms % 1000;

    printf("| %3d | %3d/%-30s | %3d.%03d | %5s |\n", current_tcb->id, current_tcb->owner->task_id, current_tcb->owner->task_name,
           csecs, chundreths, thread_state_to_name(current_tcb->state));
    for (thread* t = ready_queue.top(); t != nullptr; t = t->next) {
        uint64_t ms = t->time_elapsed / (uint64_t)1000000;
        uint32_t secs      = ms / 1000;
        uint32_t hundreths = ms % 1000;
        printf("| %3d | %3d/%-30s | %3d.%03d | %5s |\n", t->id, t->owner->task_id, t->owner->task_name, secs, hundreths,
               thread_state_to_name(t->state));
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
    t->state    = thread_state::dead;
    t->slice_ns = 0;
    schedule();
}

void scheduler::lock() { scheduler_lock++; }
void scheduler::unlock() { scheduler_lock--; }

extern "C" void sched_readd_thread(thread* t) { scheduler::enqueue(t); }
