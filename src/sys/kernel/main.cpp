#include <kernel/elf.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/vas.h>
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

typedef void startfn(void);

void test_thread() {
    // The job of this thread is to load and execute the test executable
    const char* fpath = "/apps/test_program";

    auto* file = kernel::vfs::g_vfs.find(fpath);
    if (file == nullptr) { kernel::scheduler::terminate(nullptr); }

    uint8_t* buf = new uint8_t[file->size];

    file->delegate->read(file, 0, file->size, buf);

    auto test_exe = kernel::elf_file(buf);
    test_exe.debug_print();

    for (unsigned int i = 0; i < test_exe.prog_num(); i++) {
        auto* sec = test_exe.prog_header(i);
        kernel::log::debug("elf", "ph %d: %s vaddr: 0x%08x, filesz: 0x%08x, memsz: 0x%08x\n", i, sec->type_name(), sec->p_vaddr, sec->p_filesz, sec->p_memsz);

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

/// The main kernel function.
void kernel_main() {
    // Initialise the full heap
    kernel::log::debug("heap", "Setup SLAB heap allocator\n");
    g_heap.init(true);

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

    // for (size_t i = 0; i < 10; i++) { kernel::scheduler::enqueue(new kernel::thread(newtask, (void*)&dummy_thread)); }

    kernel::log::get().flush();
    kernel::scheduler::debug();
    kernel::scheduler::unlock();  // Start scheduling processes

    while (true) { asm("hlt"); }
}
