#include <kernel/log.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/tasks/syscall.h>
#include <kernel/vfs/vfs.h>

using namespace kernel;
using namespace kernel::tasks;

bool syscall_handler::handle_interrupt(register_frame_t* regs) {
    int int_no = regs->rdi;
    log::trace("syscall", "System call 0x%x\n", int_no);

    if (int_no == 0x00) {  // _exit(int status)
        scheduler::terminate(nullptr);
    } else if (int_no == 0x01) {  // _write
        vfs::g_vfs.write(regs->rsi, (uint8_t*)regs->rdx, regs->rcx);
    } else if (int_no == 0x02) {  // _open
        std::string_view sv((const char*)regs->rsi);
        auto             id = vfs::g_vfs.open_file(sv, regs->rdx);

        log::trace("open", "%s opened with local fid: %d\n", sv.data(), id);
        regs->rax = id;
    } else {
        log::error("syscall", "Unknown system call 0x%x", int_no);
    }
    return true;
}