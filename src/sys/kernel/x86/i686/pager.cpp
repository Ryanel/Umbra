#include <kernel/log.h>
#include <kernel/mm/vmm.h>
#include <kernel/scheduler.h>
#include <kernel/x86/pager.h>

bool kernel::x86_pager::handle_interrupt(register_frame_t* regs) {
    volatile uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

    bool user    = ((regs->err_code & 0b100) != 0);
    bool write   = ((regs->err_code & 0b10) != 0);
    bool present = ((regs->err_code & 0b1) != 0);

    if (!present && !user) {
        bool demand_paging = (kernel::g_vmm.vas_current->get_page(faulting_address).raw & (1 << 9)) != 0;
        if (demand_paging) {
            kernel::log::trace("pager", "Fufilling demand paging: 0x%08x\n", faulting_address);
            kernel::g_vmm.fulfill_demand_page(faulting_address);
            return true;
        }
    }

    // Print a message
    const char* privilage_s = user ? "User" : "Kernel";
    const char* write_s     = write ? "write to " : "read";
    const char* present_s   = present ? "present" : "non-present";

    kernel::log::error("pager", "faulting addr: 0x%08x\n", faulting_address);
    kernel::log::error("pager", "%s process tried to %s a %s page\n", privilage_s, write_s, present_s);
    return false;
}