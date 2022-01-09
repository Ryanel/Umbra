#include <kernel/tasks/scheduler.h>
#include <kernel/time.h>
#include <stdio.h>
using namespace kernel;

// Data
// ----------------------------------------------------------------------------
uint64_t           time::real_time       = 0;
uint64_t           time::time_since_boot = 0;
hal::system_timer* time::system_timer    = nullptr;

// Methods
// ----------------------------------------------------------------------------
void time::increment(uint64_t ns) {
    real_time += ns;
    time_since_boot += ns;

    // Now, try and schedule a task
    tasks::scheduler::lock();
    tasks::scheduler::schedule();
    tasks::scheduler::unlock();
}

uint64_t time::boot_time_ns() { return time_since_boot; }
uint64_t time::time_ns() { return real_time; }
