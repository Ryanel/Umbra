#include <kernel/delay.h>
#include <kernel/log.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>
#include <kernel/time.h>
#include <kernel/version.h>
#include <stdio.h>

extern "C" uint32_t* stack_top;

void test_thread() {
    klogf("test thread", "test_thread() done executing, yielding\n");

    while (true) { kernel::scheduler::schedule(); }
}

/// The main kernel function.
void kernel_main() {
    auto& log = kernel::log::get();
    klogf("kernel", "Entered kmain()\n");

    // TODO: Initialise the full heap
    // Setup the scheduler
    kernel::scheduler::init(kernel::g_vmm.dir_current->directory_addr);
    kernel::scheduler::enqueue_new(kernel::threading::create((void*)&test_thread));
    kernel::scheduler::debug();

    // TODO: Create an executable.
    // TODO: Create a virtual filesystem.
    // TODO: Create filesystem drivers
    // TODO: Spawn /sbin/init and start processing messages!

    // We've exited init. Print a warning to the log and hang.
    klogf("time", "Kernel took %l ns to boot (%l ms)\n", kernel::time::boot_time_ns(),
          kernel::time::boot_time_ns() / (uint64_t)1000000);
    log.flush();

    while (true) { kernel::scheduler::schedule(); }
}