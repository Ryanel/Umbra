#pragma once

#include <kernel/panic.h>
#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/util/linked_list.h>

extern "C" void set_kernel_stack(uint32_t addr);

namespace kernel {

class vas;

class scheduler {
   public:
    static task* kernel_task;

    static void schedule();
    static void init(vas* kernel_vas);
    static void enqueue(thread* t);

    static void terminate(thread* t);
    static void sleep(thread* t, uint64_t ns);
    static void yield(thread* t);
    static void block(thread* t, int reason);
    static void unblock(thread* t, int reason);

    static void debug();
    static void debug_print_thread(thread* t);

    static void enable();
    static void disable();

    static void lock();
    static void unlock();

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
}  // namespace kernel