#include <assert.h>
#include <kernel/hal/terminal.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vas.h>
#include <kernel/mm/vmm.h>
#include <kernel/object.h>
#include <kernel/panic.h>
#include <kernel/tasks/elfloader.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/tasks/syscall.h>
#include <kernel/tasks/task.h>
#include <kernel/tasks/thread.h>
#include <kernel/time.h>
#include <kernel/vfs/initrd.h>
#include <kernel/vfs/vfs.h>
#include <kernel/x86/ps2keyboard.h>
#include <stdio.h>

using namespace kernel;
using namespace kernel::vfs;
using namespace kernel::tasks;

/// The main kernel function.
void kernel_main() {
    // Initialise the full heap
    log::debug("heap", "Setup SLAB heap allocator\n");
    g_heap.init(true);

    // Setup interrupt handler for system calls
    log::info("kernel", "Setting up System calls\n");
    interrupts::handler_register(0x80, new syscall_handler());

    // Setup the scheduler
    log::info("kernel", "Initializing the scheduler...\n");
    scheduler::init(g_cpu_data[0].current_vas);

    // Create a virtual filesystem.
    log::info("vfs", "Initiailize the virtual filesystem\n");
    vfs::g_vfs.init();

    // Load initial ramdisk
    log::info("vfs", "Loading inital ramdisk from memory\n");
    auto* initrd = new vfs::initrd_provider();
    initrd->init();

    // Start some tasks
    log::info("debug", "Starting some test tasks\n");
    auto* cloned   = g_vmm.current_vas()->clone();
    auto* task_hnd = new handle(make_ref(new task(cloned->physical_addr(), 1, "test_program")), 1, 0xFFFFFFFF, 1);
    task_hnd->as<task>()->m_directory = cloned;

    {
        auto* thread_hnd = task_hnd->as<task>()->spawn_local_thread("test", (void*)&kernel::tasks::elf_loader::load_elf,
                                                                    (uintptr_t) "/apps/test_program");
        scheduler::enqueue(thread_hnd->as<thread>().get());
    }

    // Create the console
    auto* testterm = new hal::terminal(80, 25, 4000);
    testterm->set_output(kernel::log::get().console[1]);

    auto* dev_dir = g_vfs.find("/dev/");
    auto* term    = new vfs_node(dev_dir, testterm, vfs_type::device, 0, "console");
    auto* kbd     = new vfs_node(dev_dir, new driver::ps2keyboard(), vfs_type::device, 0, "keyboard");

    kernel::log::info("main", "Reached end of kernel_main()\n");

    scheduler::unlock();  // Start scheduling processes
    testterm->refresh();
    while (true) { asm("hlt"); }
}
