#include <kernel/critical.h>
#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/vmm.h>
#include <kernel/scheduler.h>
#include <kernel/time.h>
#include <stdio.h>

using namespace kernel;

kernel::thread*      current_tcb;
kernel::thread*      reaper;
task*                current_task;
extern "C" uint32_t* stack_top;

thread idle_thread;
task   _kernel_task;

task*                            scheduler::kernel_task;
int                              scheduler::lock_queues             = 1;
int                              scheduler::lock_prevent_scheduling = 1;
bool                             scheduler::task_switch_delayed     = false;
uint64_t                         scheduler::last_schedule_ns        = 0;
util::linked_list_inline<thread> scheduler::list_ready;
util::linked_list_inline<thread> scheduler::list_sleeping;
util::linked_list_inline<thread> scheduler::list_dead;
util::linked_list_inline<thread> scheduler::list_blocked;

void thread_reaper() {
    kernel::log::info("reaper", "Reaper online!\n");

    while (true) {
        if (!scheduler::list_dead.empty()) {
            auto* dead_thread = scheduler::list_dead.pop_front();
            kernel::log::warn("reaper", "Reaped thread %d(%s)!\n", dead_thread->m_id,
                              dead_thread->m_name.value_or("anonymous"));

            g_heap.free(dead_thread->m_k_stack_top);
            delete dead_thread;
        } else {
            scheduler::block(nullptr, 1);
        }
    }
}

void scheduler::init(vas* kernel_vas) {
    last_schedule_ns = kernel::time::boot_time_ns();

    _kernel_task.m_task_id   = 0;
    _kernel_task.m_task_name = "kernel";
    _kernel_task.m_vas       = kernel_vas->physical_addr();
    _kernel_task.m_directory = kernel_vas;

    current_task           = &_kernel_task;
    scheduler::kernel_task = &_kernel_task;

    current_tcb                = &idle_thread;
    current_tcb->m_k_stack_top = (uintptr_t)&stack_top;
    current_tcb->m_state       = thread_state::running;
    current_tcb->m_id          = 0;
    current_tcb->m_owner       = kernel_task;
    current_tcb->m_priority    = 0;  // Lowest priority
    current_tcb->m_name        = optional<char*>("idle thread");

    thread_switch(&idle_thread);

    // Immediately, we'll spawn the reaper thread
    reaper = new thread(kernel_task, (void*)&thread_reaper, "reaper");
    enqueue(reaper);
}

void scheduler::schedule() {
    if (current_tcb == nullptr) { return; }

    bool allow_swap  = true;
    bool should_swap = false;

    if (lock_prevent_scheduling > 0) {
        task_switch_delayed = true;
        allow_swap          = false;
    }

    // Perform time accounting
    uint64_t elapsed = (int64_t)(kernel::time::boot_time_ns() - last_schedule_ns);
    if (elapsed > current_tcb->m_slice_ns) {
        current_tcb->m_slice_ns = 0;
    } else {
        current_tcb->m_slice_ns -= elapsed;
    }

    current_tcb->m_time_elapsed += elapsed;
    last_schedule_ns = kernel::time::boot_time_ns();

    // Unblock any sleeping processes
    for (thread* t = list_sleeping.front(); t != nullptr; t = t->m_next) {
        if (t->m_slice_ns <= last_schedule_ns) {
            list_sleeping.remove(t);
            t->m_state    = thread_state::ready_to_run;
            t->m_slice_ns = determine_timeslice(t);
            list_ready.push_back(t);
        }
    }

    if (allow_swap == false) { return; }

    // Swap if: We ran out of time
    if (current_tcb->m_slice_ns <= 0) { should_swap = true; }
    // Swap if: Process is no longer running (blocked or dead)
    if (!current_tcb->ready()) { should_swap = true; }

    if (allow_swap && should_swap) {
        if (!list_ready.empty()) {
            // Only enqueue processes that are running.
            if (current_tcb->ready()) {
                current_tcb->m_state = thread_state::ready_to_run;
                list_ready.push_back(current_tcb);
                current_tcb->m_slice_ns = 0;
            }

            auto* next                = list_ready.pop_front();
            next->m_slice_ns          = determine_timeslice(next);
            kernel::g_vmm.vas_current = next->m_owner->m_directory;
            thread_switch(next);
        }
    }
}

uint64_t kernel::scheduler::determine_timeslice(thread* t) {
    if (t->m_priority == 0) { return 10000; }
    return 50000000;
}

const char* thread_state_to_name(thread_state state) {
    switch (state) {
        case thread_state::running: return "run";
        case thread_state::ready_to_run: return "ready";
        case thread_state::dead: return "dead";
        case thread_state::blocked: return "block";
        case thread_state::sleeping: return "sleep";
        default: return "?????";
    }
}

void scheduler::debug() {
    kernel::scheduler::disable();
    uint64_t ms        = kernel::time::boot_time_ns() / (uint64_t)1000000;
    uint32_t secs      = (uint32_t)(ms / 1000);
    uint32_t hundreths = (uint32_t)(ms % 1000);

    kernel::log::debug("sched", "+---------------------------------------------------------------------+\n");
    kernel::log::debug("sched", "| Scheduler status @ %04d.%03ds after boot                             |\n", secs, hundreths);
    kernel::log::debug("sched", "|        ID / Name | TID |  Thread Name | CPU Time | State | Deadline |\n");
    kernel::log::debug("sched", "+------------------+-----+--------------+----------+-------+----------+\n");
    debug_print_thread(current_tcb);

    for (thread* t = list_ready.front(); t != nullptr; t = t->m_next) { debug_print_thread(t); }
    for (thread* t = list_sleeping.front(); t != nullptr; t = t->m_next) { debug_print_thread(t); }
    for (thread* t = list_dead.front(); t != nullptr; t = t->m_next) { debug_print_thread(t); }
    for (thread* t = list_blocked.front(); t != nullptr; t = t->m_next) { debug_print_thread(t); }

    kernel::log::debug("sched", "+---------------------------------------------------------------------+\n");
    kernel::scheduler::enable();
}

void scheduler::debug_print_thread(thread* t) {
    uint64_t ms        = t->m_time_elapsed / (uint64_t)1000000;
    uint32_t secs      = (uint32_t)(ms / 1000);
    uint32_t hundreths = (uint32_t)(ms % 1000);

    uint64_t deadline_ms        = t->m_slice_ns / (uint64_t)1000000;
    uint32_t deadline_secs      = (uint32_t)(deadline_ms / 1000);
    uint32_t deadline_hundreths = (uint32_t)(deadline_ms % 1000);

    kernel::log::debug("sched", "| %2d:%-13s | %3d | %12s | %4d.%03d | %5s | %5d.%03d|\n", t->m_owner->m_task_id,
                       t->m_owner->m_task_name, t->m_id, t->m_name.value_or("anonymous"), secs, hundreths,
                       thread_state_to_name(t->m_state), deadline_secs, deadline_hundreths);
}

void scheduler::enqueue(thread* t) {
    kernel::scheduler::disable();
    list_ready.push_back(t);
    kernel::scheduler::enable();
}

void scheduler::terminate(thread* t) {
    if (t == nullptr) { t = current_tcb; }
    t->m_state    = thread_state::dead;
    t->m_slice_ns = 0;

    list_dead.push_back(t);
    unblock(reaper, 1);
    interrupts_disable();
    schedule();
    // Thread ends here...
}

void scheduler::yield(thread* t) {
    if (t == nullptr) { t = current_tcb; }
    t->m_slice_ns = 0;
    kernel::scheduler::disable();
    schedule();
    kernel::scheduler::enable();
}

void scheduler::block(thread* t, int reason) {
    if (t == nullptr) { t = current_tcb; }
    kernel::scheduler::disable();
    t->m_blocked  = reason;
    t->m_state    = thread_state::blocked;
    t->m_slice_ns = 0;
    list_blocked.push_back(t);
    schedule();
    kernel::scheduler::enable();
}

void scheduler::unblock(thread* t, int reason) {
    if (t == nullptr) { t = current_tcb; }
    // Don't unblock tasks that aren't blocked
    if (t->ready()) { return; }
    kernel::scheduler::disable();

    // Search for the thread to unblock...
    for (thread* ti = list_blocked.front(); ti != nullptr; ti = ti->m_next) {
        if (ti == t) {
            // Remove from list blocked
            list_blocked.remove(t);
            t->m_state = thread_state::ready_to_run;
            list_ready.push_back(t);
            break;
        }
    }

    kernel::scheduler::enable();
}

void scheduler::sleep(thread* t, uint64_t ns) {
    if (t == nullptr) { t = current_tcb; }

    t->m_state    = thread_state::sleeping;
    t->m_slice_ns = ns;

    list_sleeping.push_back(t);

    schedule();
}

void scheduler::lock() {
    interrupts_disable();
    lock_queues++;
}

void scheduler::unlock() {
    lock_queues--;
    if (lock_queues == 0) { interrupts_enable(); }
}

void scheduler::disable() {
    interrupts_disable();
    // printf(">");
    lock_prevent_scheduling++;
    lock_queues++;
}

void scheduler::enable() {
    lock_prevent_scheduling--;

    if (lock_prevent_scheduling == 0) {
        if (task_switch_delayed) {
            task_switch_delayed = false;
            kernel::scheduler::lock();
            schedule();
            kernel::scheduler::unlock();
        }
    }
    lock_queues--;
    if (lock_queues == 0) { interrupts_enable(); }

    // printf("<");
}
