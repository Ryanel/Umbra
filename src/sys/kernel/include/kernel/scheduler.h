#pragma once

#include <kernel/interrupts.h>
#include <kernel/panic.h>
#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/util/linked_list.h>

namespace kernel {

class vas;

class scheduler {
   public:
    static task*   kernel_task;
    static task*   get_current_task();
    static thread* get_current_thread();

    static void schedule();
    static void init(vas* kernel_vas);

    // Thread operations
    static void enqueue(thread* t);
    static void terminate(thread* t);
    static void sleep(thread* t, uint64_t ns);
    static void yield(thread* t);
    static void block(thread* t, int reason);
    static void unblock(thread* t, int reason);

    // Debug
    static void debug();
    static void debug_print_thread(thread* t);

    // Enable and disable
    static void unlock();
    static void lock();

    static bool task_switch_delayed;

    static util::linked_list_inline<thread> list_ready;
    static util::linked_list_inline<thread> list_sleeping;
    static util::linked_list_inline<thread> list_dead;
    static util::linked_list_inline<thread> list_blocked;

   private:
    static uint64_t determine_timeslice(thread* t);
    static uint64_t last_schedule_ns;
    static int      lock_prevent_scheduling;
    static int      lock_queues;
};

struct critical_section {
    critical_section() { kernel::scheduler::lock(); }
    ~critical_section() { kernel::scheduler::unlock(); }
};

}  // namespace kernel
