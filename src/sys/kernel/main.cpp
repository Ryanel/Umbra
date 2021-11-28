#include <kernel/critical.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/scheduler.h>
#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/time.h>
#include <kernel/version.h>
#include <kernel/vfs/initrd.h>
#include <kernel/vfs/vfs.h>
#include <stdio.h>

extern "C" uint32_t* stack_top;

void test_thread() {
    klogf("test thread", "sleeping for 1000ms\n");
    kernel::scheduler::sleep(nullptr, 1000000000);
    klogf("test thread", "slept for 1000ms\n");
}

/// The main kernel function.
void kernel_main() {
    klogf("kernel", "Entered kmain()\n");

    // Initialise the full heap
    g_heap.init(true);

    // Setup the scheduler
    kernel::scheduler::init(kernel::g_vmm.dir_current->directory_addr);

    auto* new_task      = new kernel::task();
    new_task->vas       = kernel::g_vmm.dir_current->directory_addr;
    new_task->task_name = "test_task";
    new_task->task_id   = 1;
    kernel::scheduler::enqueue(new kernel::thread(new_task, (void*)&test_thread));

    // Create a virtual filesystem.
    kernel::vfs::g_vfs.init();

    kernel::vfs::initrd_provider* initrd = new kernel::vfs::initrd_provider();
    initrd->init();

    kernel::vfs::g_vfs.debug();

    // TODO: Create filesystem drivers
    // TODO: Spawn /sbin/init and start processing messages!

    // We've exited init. Print a warning to the log and hang.
    klogf("kernel", "Kernel took %l ns to boot (%l ms)\n", kernel::time::boot_time_ns(),
          kernel::time::boot_time_ns() / (uint64_t)1000000);

    kernel::log::get().flush();
    kernel::scheduler::unlock();  // Unlock the scheduler for the first time.

    while (true) {}
}
