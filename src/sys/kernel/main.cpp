#include <kernel/delay.h>
#include <kernel/log.h>
#include <kernel/panic.h>
#include <kernel/time.h>
#include <kernel/version.h>
#include <stdio.h>

/// The main kernel function.
void kernel_main() {
    auto& log = kernel::log::get();
    klogf("kernel", "Entered kmain()\n");

    // TODO: Initialise the full heap
    // TODO: Setup a scheduler
    // TODO: Create an executable.
    // TODO: Create a virtual filesystem.
    // TODO: Create filesystem drivers
    // TODO: Spawn /sbin/init and start processing messages!

    // We've exited init. Print a warning to the log and hang.
    klogf("time", "Kernel took %l ns to boot (%l ms)\n", kernel::time::boot_time_ns(), kernel::time::boot_time_ns()  / (uint64_t)1000000);
    log.flush();
    panic("No processes available to run. Halting.");
}