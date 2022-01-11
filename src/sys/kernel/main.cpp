#include <assert.h>
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

class terminal_delegate : public vfs_delegate {
   public:
    terminal_delegate() {}
    int read(vfs::vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }
    int write(vfs::vfs_node* node, size_t offset, size_t size, uint8_t* buffer) {
        for (size_t i = 0; i < size; i++) { log::get().write(buffer[i]); }
        return 0;
    }
    char const* delegate_name() { return "terminal delegate"; }
};

class keyboard_delegate : public vfs_delegate {
   public:
    keyboard_delegate(kernel::hal::keyboard* kb) : m_kb(kb) {}
    int read(vfs::vfs_node* node, size_t offset, size_t size, uint8_t* buffer) {
        int elements_copied = 0;
        while (elements_copied < size && m_kb->event_buffer.size() > 0) {
            auto evnt = m_kb->event_buffer.top();
            if (evnt.pressed) {
                buffer[offset + elements_copied] = evnt.keycode;
                elements_copied++;
            }
            m_kb->event_buffer.pop();
        }
        return elements_copied;
    }
    int         write(vfs::vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }
    char const* delegate_name() { return "keyboard delegate"; }

   private:
    kernel::hal::keyboard* m_kb;
};

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
                                                                    (uintptr_t)"/apps/test_program");
        scheduler::enqueue(thread_hnd->as<thread>().get());
    }

    // Create the console
    auto* dev_dir = vfs::g_vfs.find("/dev/");
    auto* term    = new vfs::vfs_node(dev_dir, new terminal_delegate(), vfs::vfs_type::device, 0);
    term->set_name("console");

    auto* kb = new kernel::driver::ps2keyboard();
    kb->init();

    auto* kbd = new vfs::vfs_node(dev_dir, new keyboard_delegate(kb), vfs::vfs_type::device, 0);
    kbd->set_name("keyboard");

    kernel::log::info("main", "Reached end of kernel_main()\n");
    log::get().flush();

    scheduler::unlock();  // Start scheduling processes
    while (true) { asm("hlt"); }
}
