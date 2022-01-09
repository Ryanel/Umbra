#include <kernel/log.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/tasks/syscall.h>
#include <kernel/vfs/vfs.h>

using namespace kernel;
using namespace kernel::tasks;

bool syscall_handler::handle_interrupt(register_frame_t* regs) {
#ifdef ARCH_X86
    int int_no = regs->eax;
#else
    int int_no = regs->rax;
#endif
    if (int_no == 0x00) {
        log::trace("syscall", "exit()\n");
        scheduler::terminate(nullptr);
    } else if (int_no == 0x01) {
        log::trace("syscall", "write()\n");
        vfs::g_vfs.write(regs->rax, (uint8_t*)regs->rcx, regs->rdx);
    } else {
        log::error("syscall", "Unknown system call 0x%x", int_no);
    }
    return true;
}