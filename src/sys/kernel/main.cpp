#include <kernel/delay.h>
#include <kernel/log.h>
#include <kernel/version.h>
#include <kernel/panic.h>
#include <stdio.h>

/// The main kernel function.
void kernel_main() {
    auto& log = kernel::log::get();
    klogf("kernel", "Entered kmain()\n");

    // TODO: Create a VMM and PMM
    // TODO: Setup an initial 'heap' area.
    // TODO: Setup a scheduler
    // TODO: Create an executable.
    // TODO: Create a virtual filesystem.
    // TODO: Create filesystem drivers
    // TODO: Spawn /sbin/init and start processing messages!

    // We've exited init. Print a warning to the log and hang.
    log.flush();
    panic("No processes available to run. Halting.");
}