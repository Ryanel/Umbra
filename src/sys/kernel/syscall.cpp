#include <kernel/log.h>
#include <kernel/scheduler.h>
#include <kernel/vfs/vfs.h>
#include <kernel/syscall.h>

bool kernel::syscall_handler::handle_interrupt(register_frame_t* regs) {
    if (regs->eax == 0x00) {
        kernel::log::trace("syscall", "exit()\n");
        kernel::scheduler::terminate(nullptr);
    } else if (regs->eax == 0x01) {
        kernel::log::trace("syscall", "write()\n");
        kernel::vfs::g_vfs.write(regs->ebx, (uint8_t*)regs->ecx, regs->edx);
    } else {
        kernel::log::error("syscall", "Unknown system call 0x%x", regs->eax);
    }
    return true;
}