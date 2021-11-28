#pragma once

#include <kernel/panic.h>
#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/util/linked_list.h>

namespace kernel {
class scheduler {
   public:
    static task* kernel_task;

    static void schedule();
    static void init(phys_addr_t kernel_vas);
    static void enqueue(thread* t);

    static void terminate(thread* t);
    static void sleep(thread *t, uint64_t ns);
    static void yield(thread *t);
    
    static void debug();
    static void debug_print_thread(thread * t);

    static void lock();
    static void unlock();


   private:
    static uint64_t determine_timeslice(thread * t);
    
    static util::linked_list_inline<thread> list_ready;
    static util::linked_list_inline<thread> list_sleeping;
    
    static uint64_t        last_schedule_ns;
    static int             scheduler_lock;
};
}  // namespace kernel