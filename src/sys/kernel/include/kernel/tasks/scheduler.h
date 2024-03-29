#pragma once

#include <kernel/interrupts.h>
#include <kernel/panic.h>
#include <kernel/tasks/task.h>
#include <kernel/tasks/thread.h>

#include <list>

namespace kernel {
namespace tasks {
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

    static std::list<thread*> list_ready;
    static std::list<thread*> list_sleeping;
    static std::list<thread*> list_dead;
    static std::list<thread*> list_blocked;

   private:
    static uint64_t determine_timeslice(thread* t);
    static uint64_t last_schedule_ns;
    static int      lock_prevent_scheduling;
    static int      lock_queues;
};

}  // namespace tasks
}  // namespace kernel
