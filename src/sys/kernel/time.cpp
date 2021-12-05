#include <kernel/scheduler.h>
#include <kernel/time.h>

// Data
// ----------------------------------------------------------------------------
uint64_t                   kernel::time::real_time       = 0;
uint64_t                   kernel::time::time_since_boot = 0;
kernel::hal::system_timer* kernel::time::system_timer    = nullptr;

// Methods
// ----------------------------------------------------------------------------
void kernel::time::increment(uint64_t ns) {
    real_time += ns;
    time_since_boot += ns;

    // Now, try and schedule a task
    kernel::scheduler::lock();
    kernel::scheduler::schedule();
    kernel::scheduler::unlock();
}

uint64_t kernel::time::boot_time_ns() { return time_since_boot; }
uint64_t kernel::time::time_ns() { return real_time; }
