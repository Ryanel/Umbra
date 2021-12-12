#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/pmm.h>
#include <kernel/mm/vas.h>
#include <kernel/mm/vmm.h>
#include <kernel/object.h>
#include <kernel/panic.h>
#include <kernel/tasks/elf.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/tasks/syscall.h>
#include <kernel/tasks/task.h>
#include <kernel/tasks/thread.h>
#include <kernel/time.h>
#include <kernel/vfs/initrd.h>
#include <kernel/vfs/vfs.h>
#include <stdio.h>

typedef void startfn(void);

using namespace kernel;
using namespace kernel::vfs;
using namespace kernel::tasks;

void test_thread() {
    // The job of this thread is to load and execute the test executable
    const char* fpath = "/apps/test_program";

    // Load file
    auto fd = g_vfs.open_file(fpath, 0);
    if (fd == -1) { scheduler::terminate(nullptr); }
    auto  size = g_vfs.fstat(fd).size;
    auto* buf = new uint8_t[((size + 0x1000) & ~(PAGE_SIZE - 1))];  // Allocate a buffer that's page sized bytes long to
                                                                    // not make unnessisary slabs.
    vfs::g_vfs.read(fd, buf, size);

    // Open a console for the program
    auto term = vfs::g_vfs.open_file("/dev/console", 0);

    // Parse as an ELF
    auto test_exe = elf_file(buf);

    for (unsigned int i = 0; i < test_exe.prog_num(); i++) {
        auto* sec = test_exe.prog_header(i);
        // Load
        if (sec->p_type == 0x1) {
            g_vmm.mmap(sec->p_vaddr, sec->p_memsz, VMM_PROT_USER | VMM_PROT_WRITE, VMM_FLAG_POPULATE);
            memset((void*)sec->p_vaddr, 0, sec->p_memsz);
            memcpy((void*)sec->p_vaddr, (void*)((uintptr_t)test_exe.m_header + sec->p_offset), sec->p_filesz);
        }
    }

    // At this point, the program is ready to be executed. Call it.
    log::info("elfloader", "Launching %s\n", fpath);
    startfn* fn = (startfn*)(test_exe.m_header->e_entry);
    fn();
}

void dummy_thread() {
    int counter = 0;
    while (counter < 5000) {
        counter++;
        scheduler::yield(nullptr);
    }
}

class terminal_delegate : public vfs_delegate {
   public:
    terminal_delegate() {}
    int read(vfs::vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }
    int write(vfs::vfs_node* node, size_t offset, size_t size, uint8_t* buffer) {
        for (size_t i = 0; i < size; i++) { log::get().write(buffer[i]); }
        return 0;
    }
    char const* delegate_name() { return "null delegate"; }
};

/// The main kernel function.
void kernel_main() {
    // Initialise the full heap
    log::debug("heap", "Setup SLAB heap allocator\n");
    g_heap.init(true);
    kernel::g_pmm.print_statistics();
    // Setup interrupt handler for system calls
    log::info("kernel", "Setting up System calls\n");
    interrupts::handler_register(0x80, new syscall_handler());

    // Setup the scheduler
    log::info("kernel", "Initializing the scheduler...\n");
    scheduler::init(g_vmm.vas_current);

    // Create a virtual filesystem.
    log::info("vfs", "Initiailize the virtual filesystem\n");
    vfs::g_vfs.init();

    // Load initial ramdisk
    log::info("vfs", "Loading inital ramdisk from memory\n");
    auto* initrd = new vfs::initrd_provider();
    initrd->init();

    // Start some tasks
    log::info("debug", "Starting some test tasks\n");
    auto* cloned         = g_vmm.current_vas()->clone();
    auto* newtask        = new task(cloned->physical_addr(), 1, "test_program");
    newtask->m_directory = cloned;
    scheduler::enqueue(new thread(newtask, (void*)&test_thread, "test main"));

    // Create the console
    auto* dev_dir = vfs::g_vfs.find("/dev/");
    auto* term    = new vfs::vfs_node(dev_dir, new terminal_delegate(), vfs::vfs_type::device, 0);
    term->set_name("console");

    kernel::g_pmm.print_statistics();

    log::get().flush();
    scheduler::unlock();  // Start scheduling processes

    while (true) { asm("hlt"); }
}
