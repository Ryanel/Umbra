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
    int int_no = regs->rdi;
#endif
    log::trace("syscall", "System call 0x%x\n", int_no);
    if (int_no == 0x00) {
        scheduler::terminate(nullptr);
    } else if (int_no == 0x01) {
        vfs::g_vfs.write(regs->rsi, (uint8_t*)regs->rdx, regs->rcx);
    } else {
        log::error("syscall", "Unknown system call 0x%x", int_no);
    }
    return true;
}