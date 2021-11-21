#include <kernel/time.h>
#include <kernel/scheduler.h>

uint64_t                   kernel::time::real_time       = 0;
uint64_t                   kernel::time::time_since_boot = 0;
kernel::hal::system_timer* kernel::time::system_timer    = nullptr;

void kernel::time::increment(uint64_t ns) {
    real_time += ns;
    time_since_boot += ns;
}

uint64_t kernel::time::boot_time_ns() { return time_since_boot; }
uint64_t kernel::time::time_ns() { return real_time; }
