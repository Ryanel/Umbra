#include <kernel/delay.h>
#include <kernel/log.h>
#include <kernel/version.h>
#include <kernel/panic.h>
#include <stdio.h>
/// The main kernel function.
void kernel_main() {
    auto& log = kernel::log::get();
    kprintf("Entered kernel main\n");
    kernel_print_version();

    // TODO: Create a VMM and PMM
    // TODO: Setup an initial 'heap' area.
    // TODO: Setup a scheduler
    // TODO: Create an executable.
    // TODO: Create a virtual filesystem.
    // TODO: Create filesystem drivers
    // TODO: Spawn /sbin/init and start processing messages!

    // We've exited init. Print a warning to the log and hang.
    kprintf("No processes running. Entering infinite loop.\n");

    // Ensure that the log is flushed before we hang! We could miss out on output if not done.
    log.flush();

    // Hang
    panic("No running processes");
}