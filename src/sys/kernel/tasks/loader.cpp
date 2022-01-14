#include <assert.h>
#include <kernel/log.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/vas.h>
#include <kernel/mm/vmm.h>
#include <kernel/tasks/elf.h>
#include <kernel/tasks/elfloader.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/vfs/vfs.h>

using namespace kernel;
using namespace kernel::vfs;
using namespace kernel::tasks;

typedef void startfn(void);
extern "C" void enter_usermode(uintptr_t ptr, uintptr_t stack);

void kernel::tasks::elf_loader::load_elf(const char * fpath) {
    // Load file
    auto fd = g_vfs.open_file(fpath, 0);

    if (fd == -1) {
        log::error("test_thread", "Unable to find %s\n", fpath);
        scheduler::terminate(nullptr);
    }

    auto  size = g_vfs.fstat(fd).size;
    auto* buf = new uint8_t[((size + 0x1000) & ~(PAGE_SIZE - 1))];  // Allocate a buffer that's page sized bytes long to
                                                                    // not make unnessisary slabs.
    vfs::g_vfs.read(fd, buf, size);

    // Open a console for the program
    auto term = vfs::g_vfs.open_file("/dev/console", 0);

    // Parse as an ELF
    auto test_exe = elf64_file(buf);

    assert(test_exe.valid() == true);
    log::info("elfloader", "Mapping PHs\n");
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

    //virt_addr_t stack = (virt_addr_t)g_vmm.mmap(0xf0000000, 0x4000, VMM_PROT_WRITE | VMM_PROT_USER, VMM_FLAG_POPULATE);
    //log::info("elfloader", "Entry: 0x%016p, stack: 0x%016p\n", test_exe.m_header->e_entry, stack);
    //enter_usermode(test_exe.m_header->e_entry, stack);
}