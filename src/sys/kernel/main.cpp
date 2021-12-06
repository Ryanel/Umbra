#include <kernel/elf.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/vas.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/scheduler.h>
#include <kernel/syscall.h>
#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/time.h>
#include <kernel/vfs/initrd.h>
#include <kernel/vfs/vfs.h>
#include <stdio.h>

extern "C" uint32_t* stack_top;

typedef void startfn(void);

void test_thread() {
    // The job of this thread is to load and execute the test executable
    const char* fpath = "/apps/test_program";

    // Load file
    auto fd = kernel::vfs::g_vfs.open_file(fpath, 0);
    if (fd == -1) { kernel::scheduler::terminate(nullptr); }
    auto  size = kernel::vfs::g_vfs.fstat(fd).size;
    auto* buf = new uint8_t[((size + 0x1000) & ~(PAGE_SIZE - 1))];  // Allocate a buffer that's page sized bytes long to
                                                                    // not make unnessisary slabs.
    kernel::vfs::g_vfs.read(fd, buf, size);

    // Open a console for the program
    auto term = kernel::vfs::g_vfs.open_file("/dev/console", 0);

    // Parse as an ELF
    auto test_exe = kernel::elf_file(buf);

    for (unsigned int i = 0; i < test_exe.prog_num(); i++) {
        auto* sec = test_exe.prog_header(i);
        // Load
        if (sec->p_type == 0x1) {
            kernel::g_vmm.mmap(sec->p_vaddr, sec->p_memsz, VMM_PROT_USER | VMM_PROT_WRITE, VMM_FLAG_POPULATE);
            memset((void*)sec->p_vaddr, 0, sec->p_memsz);
            memcpy((void*)sec->p_vaddr, (void*)((uintptr_t)test_exe.m_header + sec->p_offset), sec->p_filesz);
        }
    }

    // At this point, the program is ready to be executed. Call it.
    kernel::log::info("elfloader", "Launching %s\n", fpath);
    startfn* fn = (startfn*)(test_exe.m_header->e_entry);
    fn();
}

void dummy_thread() {
    int counter = 0;
    while (counter < 5000) {
        counter++;
        kernel::scheduler::yield(nullptr);
    }
}

class terminal_delegate : public kernel::vfs::vfs_delegate {
   public:
    terminal_delegate() {}
    int read(kernel::vfs::vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }
    int write(kernel::vfs::vfs_node* node, size_t offset, size_t size, uint8_t* buffer) {
        for (size_t i = 0; i < size; i++) { kernel::log::get().write(buffer[i]); }
        return 0;
    }
    char const* delegate_name() { return "null delegate"; }
};

/// The main kernel function.
void kernel_main() {
    // Initialise the full heap
    kernel::log::debug("heap", "Setup SLAB heap allocator\n");
    g_heap.init(true);

    // Setup interrupt handler for system calls
    kernel::log::info("kernel", "Setting up System calls\n");
    kernel::interrupts::handler_register(0x80, new kernel::syscall_handler());

    // Setup the scheduler
    kernel::log::info("kernel", "Initializing the scheduler...\n");
    kernel::scheduler::init(kernel::g_vmm.vas_current);

    // Create a virtual filesystem.
    kernel::log::info("vfs", "Initiailize the virtual filesystem\n");
    kernel::vfs::g_vfs.init();

    // Load initial ramdisk
    kernel::log::info("vfs", "Loading inital ramdisk from memory\n");
    auto* initrd = new kernel::vfs::initrd_provider();
    initrd->init();

    // Start some tasks
    kernel::log::info("debug", "Starting some test tasks\n");
    auto* cloned         = kernel::g_vmm.current_vas()->clone();
    auto* newtask        = new kernel::task(cloned->physical_addr(), 1, "test_program");
    newtask->m_directory = cloned;
    kernel::scheduler::enqueue(new kernel::thread(newtask, (void*)&test_thread, "test main"));

    auto* dev_dir = kernel::vfs::g_vfs.find("/dev/");
    auto* term    = new kernel::vfs::vfs_node(dev_dir, new terminal_delegate(), kernel::vfs::vfs_type::device, 0);
    
    term->name_buffer[0] = 'c';
    term->name_buffer[1] = 'o';
    term->name_buffer[2] = 'n';
    term->name_buffer[3] = 's';
    term->name_buffer[4] = 'o';
    term->name_buffer[5] = 'l';
    term->name_buffer[6] = 'e';
    term->name_buffer[7] = '\0';

    kernel::log::get().flush();
    kernel::scheduler::unlock();  // Start scheduling processes

    while (true) { asm("hlt"); }
}
