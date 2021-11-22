#include <kernel/critical.h>
#include <kernel/delay.h>
#include <kernel/log.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/scheduler.h>
#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/time.h>
#include <kernel/version.h>
#include <stdio.h>

extern "C" uint32_t* stack_top;

void test_thread() {
    klogf("test thread", "test_thread()\n");
}

/// The main kernel function.
void kernel_main() {
    klogf("kernel", "Entered kmain()\n");

    // TODO: Initialise the full heap

    // Setup the scheduler
    kernel::scheduler::init(kernel::g_vmm.dir_current->directory_addr);
    kernel::scheduler::lock();

    auto* new_task      = new kernel::task();
    new_task->vas       = kernel::g_vmm.dir_current->directory_addr;
    new_task->task_name = "test_task";
    new_task->task_id   = 1;
    kernel::scheduler::enqueue_new(kernel::threading::create(new_task, (void*)&test_thread));

    // TODO: Create an executable.
    // TODO: Create a virtual filesystem.
    // TODO: Create filesystem drivers
    // TODO: Spawn /sbin/init and start processing messages!

    // We've exited init. Print a warning to the log and hang.
    klogf("kernel", "Kernel took %l ns to boot (%l ms)\n", kernel::time::boot_time_ns(),
          kernel::time::boot_time_ns() / (uint64_t)1000000);

    kernel::log::get().flush();
    kernel::scheduler::unlock();

    while (true) {}
}
