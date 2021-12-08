#include <kernel/log.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/tasks/syscall.h>
#include <kernel/vfs/vfs.h>

using namespace kernel;
using namespace kernel::tasks;

bool syscall_handler::handle_interrupt(register_frame_t* regs) {
    if (regs->eax == 0x00) {
        log::trace("syscall", "exit()\n");
        scheduler::terminate(nullptr);
    } else if (regs->eax == 0x01) {
        log::trace("syscall", "write()\n");
        vfs::g_vfs.write(regs->ebx, (uint8_t*)regs->ecx, regs->edx);
    } else {
        log::error("syscall", "Unknown system call 0x%x", regs->eax);
    }
    return true;
}