#include <kernel/log.h>
#include <kernel/tasks/scheduler.h>
#include <kernel/tasks/syscall.h>
#include <kernel/vfs/vfs.h>

using namespace kernel;
using namespace kernel::tasks;

bool syscall_handler::handle_interrupt(register_frame_t* regs) {
    int int_no = regs->rdi;
    // log::trace("syscall", "System call 0x%x\n", int_no);

    if (int_no == 0x00) {  // _exit(int status)
        scheduler::terminate(nullptr);
    } else if (int_no == 0x01) {  // _write
        log::trace("write", "Writing using handle %d\n", regs->rsi);
        handle* hnd = scheduler::get_current_task()->m_local_handles.get((uint32_t)regs->rsi);
        regs->rax   = vfs::g_vfs.write(hnd, (uint8_t*)regs->rdx, regs->rcx);
    } else if (int_no == 0x02) {  // _open
        std::string_view sv((const char*)regs->rsi);
        auto             id = vfs::g_vfs.open_file(sv, regs->rdx);

        if (id != nullptr) {
            regs->rax = id->m_id;
            log::trace("open", "%s opened with handle: %d\n", sv.data(), regs->rax);
        } else {
            regs->rax = (uint64_t)-1;
            log::warn("open", "%s failed\n", sv.data());
        }
    } else if (int_no == 0x03) {  // _read
        //log::trace("read", "Reading from handle %d\n", regs->rsi);
        handle* hnd = scheduler::get_current_task()->m_local_handles.get((uint32_t)regs->rsi);
        regs->rax   = vfs::g_vfs.read(hnd, (uint8_t*)regs->rdx, regs->rcx);
    } else {
        log::error("syscall", "Unknown system call 0x%x", int_no);
    }
    return true;
}