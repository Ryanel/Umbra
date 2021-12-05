#include <kernel/log.h>
#include <kernel/scheduler.h>
#include <kernel/syscall.h>

bool kernel::syscall_handler::handle_interrupt(register_frame_t* regs) {
    if (regs->eax == 0x00) {
        kernel::scheduler::terminate(nullptr);
    } else {
        kernel::log::error("syscall", "Unknown system call 0x%x", regs->eax);
    }
    return true;
}