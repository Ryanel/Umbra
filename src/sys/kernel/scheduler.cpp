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

task*                     scheduler::kernel_task;
int                       scheduler::scheduler_lock   = 1;
uint64_t                  scheduler::last_schedule_ns = 0;
util::linked_list<thread> scheduler::list_ready;
util::linked_list<thread> scheduler::list_sleeping;

void scheduler::init(phys_addr_t kernel_vas) {
    _kernel_task.task_id   = 0;
    _kernel_task.task_name = "idle";
    _kernel_task.vas       = kernel_vas;

    current_task           = &_kernel_task;
    scheduler::kernel_task = &_kernel_task;

    current_tcb              = &idle_thread;
    current_tcb->k_stack_top = (uintptr_t)&stack_top;
    current_tcb->state       = thread_state::running;
    current_tcb->id          = 0;
    current_tcb->owner       = kernel_task;
    current_tcb->priority    = 0;  // Lowest priority

    last_schedule_ns = 0;

    critical_section cs;
    thread_switch(&idle_thread);
}

void scheduler::schedule() {
    critical_section cs;

    if (current_tcb == nullptr) { return; }

    const bool allow_swap_process = scheduler_lock == 0;
    bool       state_should_swap  = false;

    // Update slice accounting
    uint64_t elapsed = (int64_t)(kernel::time::boot_time_ns() - last_schedule_ns);

    if (elapsed > current_tcb->slice_ns) {
        current_tcb->slice_ns = 0;
    } else {
        current_tcb->slice_ns -= elapsed;
    }

    current_tcb->time_elapsed += elapsed;
    last_schedule_ns = kernel::time::boot_time_ns();

    // Unblock any processes we need to!
    for (thread* t = list_sleeping.front(); t != nullptr; t = t->next) {
        if (t->slice_ns <= last_schedule_ns) {
            list_sleeping.remove(t);

            t->state    = thread_state::ready_to_run;
            t->slice_ns = determine_timeslice(t);
            list_ready.push_back(t);

            // TODO: Fix skip on scheduling.
        }
    }
    // Swap if: We ran out of time
    if (current_tcb->slice_ns <= 0) { state_should_swap = true; }
    // Swap if: Process is no longer running (blocked or dead)
    if (current_tcb->state != thread_state::running && current_tcb->state != thread_state::ready_to_run) {
        state_should_swap = true;
    }

    // Swap processes.
    if (state_should_swap && allow_swap_process && !list_ready.empty()) {
        debug();

        // Only enqueue processes that are running.
        if (current_tcb->state == thread_state::running || current_tcb->state == thread_state::ready_to_run) {
            current_tcb->state = thread_state::ready_to_run;
            list_ready.push_back(current_tcb);
            current_tcb->slice_ns = 0;
        }

        if (!list_ready.empty()) {
            auto* next     = list_ready.pop_front();
            next->slice_ns = determine_timeslice(next);
            thread_switch(next);
        }
    }
}

uint64_t kernel::scheduler::determine_timeslice(thread* t) {
    // TODO: Improve, this is a terrible calculation
    return 10000000 * (t->priority + 1);
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

    uint64_t ms        = kernel::time::boot_time_ns() / (uint64_t)1000000;
    uint32_t secs      = ms / 1000;
    uint32_t hundreths = ms % 1000;

    printf("+-----------------------------------------------------------+\n");
    printf("| Scheduler status @ %04d.%03ds after boot                   |\n", secs, hundreths);

    printf("|%22s | %4s | %8s | %5s | %8s|\n", "ID / Name", "TID", "CPU Time", "State", "Deadline");
    printf("+-----------------------+------+----------+-------+---------+\n");
    debug_print_thread(current_tcb);

    for (thread* t = list_ready.front(); t != nullptr; t = t->next) { debug_print_thread(t); }
    for (thread* t = list_sleeping.front(); t != nullptr; t = t->next) { debug_print_thread(t); }

    printf("+-----------------------------------------------------------+\n");
    unlock();
}

void scheduler::debug_print_thread(thread* t) {
    uint64_t ms        = t->time_elapsed / (uint64_t)1000000;
    uint32_t secs      = ms / 1000;
    uint32_t hundreths = ms % 1000;

    uint64_t deadline_ms        = t->slice_ns / (uint64_t)1000000;
    uint32_t deadline_secs      = deadline_ms / 1000;
    uint32_t deadline_hundreths = deadline_ms % 1000;

    printf("|%2d/%-19s | %4d | %4d.%03d | %5s | %4d.%03d|\n", t->owner->task_id, t->owner->task_name, t->id, secs, hundreths,
           thread_state_to_name(t->state), deadline_secs, deadline_hundreths);
}

void scheduler::enqueue(thread* t) {
    critical_section cs;
    list_ready.push_back(t);
}

void scheduler::terminate(thread* t) {
    if (t == nullptr) { t = current_tcb; }
    t->state    = thread_state::dead;
    t->slice_ns = 0;
    schedule();
}

void scheduler::yield(thread* t) {
    if (t == nullptr) { t = current_tcb; }
    t->slice_ns = 0;
    schedule();
}

void scheduler::sleep(thread* t, uint64_t ns) {
    if (t == nullptr) { t = current_tcb; }

    t->state    = thread_state::sleeping;
    t->slice_ns = ns;

    list_sleeping.push_back(t);
    schedule();
}

void scheduler::lock() { scheduler_lock++; }
void scheduler::unlock() { scheduler_lock--; }

extern "C" void sched_readd_thread(thread* t) { scheduler::enqueue(t); }
