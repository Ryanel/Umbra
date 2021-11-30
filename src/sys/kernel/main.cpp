#include <kernel/critical.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/scheduler.h>
#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/time.h>
#include <kernel/vfs/initrd.h>
#include <kernel/vfs/vfs.h>
#include <stdio.h>

extern "C" uint32_t* stack_top;

void test_thread() {
    kernel::log::info("test thread", "sleeping for 1000ms\n");
    kernel::scheduler::sleep(nullptr, 1000000000);
    kernel::log::error("test thread", "slept for 1000ms\n");
}

/// The main kernel function.
void kernel_main() {
    kernel::log::info("kernel", "Entered kmain()\n");

    // Initialise the full heap
    g_heap.init(true);

    // Setup the scheduler
    kernel::scheduler::init(kernel::g_vmm.dir_current->directory_addr);

    // Create a sample task
    auto* new_task = new kernel::task(kernel::g_vmm.dir_current->directory_addr, 1, "test_task");
    kernel::scheduler::enqueue(new kernel::thread(new_task, (void*)&test_thread));

    // Create a virtual filesystem.
    kernel::vfs::g_vfs.init();

    // Load initial ramdisk
    kernel::log::info("kernel","Loading inital ramdisk from memory\n");
    auto* initrd = new kernel::vfs::initrd_provider();
    initrd->init();

    // TODO: Create filesystem drivers
    // TODO: Spawn /sbin/init and start processing messages!

    // We've exited init. Print a warning to the log and hang.
    kernel::log::info("kernel", "Kernel took %l ns to boot (%l ms)\n", kernel::time::boot_time_ns(),
                      kernel::time::boot_time_ns() / (uint64_t)1000000);

    kernel::vfs::g_vfs.debug();
    g_heap.debug();
    kernel::scheduler::debug();

    kernel::log::get().flush();
    kernel::scheduler::unlock();  // Unlock the scheduler for the first time.

    while (true) {}
}
