#define DEF_INT_HANDLERS

#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/panic.h>
#include <kernel/scheduler.h>
#include <kernel/time.h>
#include <kernel/x86/interrupts.h>
#include <kernel/x86/ports.h>
#include <string.h>

void interrupts_disable() { asm("cli"); }
void interrupts_enable() { asm("sti"); }
void interrupts_after_thread() { outb(0x20, 0x20); }

extern "C" void k_exception_handler(register_frame_t* regs) {
    if (regs->int_no == 14) {
        volatile uint32_t faulting_address;
        asm volatile("mov %%cr2, %0" : "=r"(faulting_address));
        kernel::log::error("paging", "faulting addr: 0x%08x\n", faulting_address);

        bool user    = ((regs->err_code & 0b100) != 0);
        bool write   = ((regs->err_code & 0b10) != 0);
        bool present = ((regs->err_code & 0b1) != 0);

        const char* privilage_s = user ? "User" : "Kernel";
        const char* write_s     = write ? "write to " : "read";
        const char* present_s   = present ? "present" : "non-present";

        kernel::log::error("paging", "%s process tried to %s a %s page\n", privilage_s, write_s, present_s);
    }

    kernel::log::error("error", "eip: 0x%08x int:%02x err:%08x eflags:%08x\n", regs->eip, regs->int_no, regs->err_code,
                       regs->eflags);
    kernel::log::error("error", "cs:%02x ds:%02x es:0x%02x fs:%02x gs:%02x ss:%02x\n", regs->cs, regs->ds, regs->es, regs->fs,
                       regs->gs, regs->ss);
    kernel::log::error("error", "eax:%08x ebx:%08x ecx:%08x edx:%08x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
    kernel::log::error("error", "ebp:%08x esp:%08x esi:%08x edi:%08x\n", regs->edi, regs->esi, regs->esp, regs->ebp);
    panic("Unhandled exception");
}
extern "C" void k_irq_handler(register_frame_t* regs) {
    // Signal interrupt handled

    if (regs->int_no == 33) {
        unsigned char scan_code = inb(0x60);
        kernel::log::warn("irq", "Keyboard IRQ: %d\n", scan_code);

            kernel::scheduler::disable();
            kernel::scheduler::debug();
            kernel::scheduler::enable();

        // Reset keyboard
    } else if (regs->int_no == 32) {
        if (kernel::time::system_timer != nullptr) {
            kernel::time::system_timer->tick();

            kernel::scheduler::disable();
            kernel::scheduler::schedule();
            kernel::scheduler::enable();
        }
    } else if (regs->int_no == 0x80) {
        kernel::log::info("syscall", "Syscall %d!\n", regs->eax);
        if (regs->eax == 0x0) {
            kernel::log::info("syscall", "Syscall exit!\n");
            kernel::scheduler::terminate(nullptr);
        }
    } else {
        kernel::log::warn("irq", "Unhandled IRQ%x\n", regs->int_no - 32);
    }

    if (regs->int_no >= 40) { outb(0xA0, 0x20); }
    outb(0x20, 0x20);
}

void x86_idt::init() {
    idtptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idtptr.base  = (uint32_t)&idt;

    memset(&idt, 0, sizeof(idt_entry_t) * 256);

    set_gate(0, (unsigned)interrupt_isr0, 0x08, 0x8E);
    set_gate(1, (unsigned)interrupt_isr1, 0x08, 0x8E);
    set_gate(2, (unsigned)interrupt_isr2, 0x08, 0x8E);
    set_gate(3, (unsigned)interrupt_isr3, 0x08, 0x8E);
    set_gate(4, (unsigned)interrupt_isr4, 0x08, 0x8E);
    set_gate(5, (unsigned)interrupt_isr5, 0x08, 0x8E);
    set_gate(6, (unsigned)interrupt_isr6, 0x08, 0x8E);
    set_gate(7, (unsigned)interrupt_isr7, 0x08, 0x8E);

    set_gate(8, (unsigned)interrupt_isr8, 0x08, 0x8E);
    set_gate(9, (unsigned)interrupt_isr9, 0x08, 0x8E);
    set_gate(10, (unsigned)interrupt_isr10, 0x08, 0x8E);
    set_gate(11, (unsigned)interrupt_isr11, 0x08, 0x8E);
    set_gate(12, (unsigned)interrupt_isr12, 0x08, 0x8E);
    set_gate(13, (unsigned)interrupt_isr13, 0x08, 0x8E);
    set_gate(14, (unsigned)interrupt_isr14, 0x08, 0x8E);
    set_gate(15, (unsigned)interrupt_isr15, 0x08, 0x8E);

    set_gate(16, (unsigned)interrupt_isr16, 0x08, 0x8E);
    set_gate(17, (unsigned)interrupt_isr17, 0x08, 0x8E);
    set_gate(18, (unsigned)interrupt_isr18, 0x08, 0x8E);
    set_gate(19, (unsigned)interrupt_isr19, 0x08, 0x8E);
    set_gate(20, (unsigned)interrupt_isr20, 0x08, 0x8E);
    set_gate(21, (unsigned)interrupt_isr21, 0x08, 0x8E);
    set_gate(22, (unsigned)interrupt_isr22, 0x08, 0x8E);
    set_gate(23, (unsigned)interrupt_isr23, 0x08, 0x8E);

    set_gate(24, (unsigned)interrupt_isr24, 0x08, 0x8E);
    set_gate(25, (unsigned)interrupt_isr25, 0x08, 0x8E);
    set_gate(26, (unsigned)interrupt_isr26, 0x08, 0x8E);
    set_gate(27, (unsigned)interrupt_isr27, 0x08, 0x8E);
    set_gate(28, (unsigned)interrupt_isr28, 0x08, 0x8E);
    set_gate(29, (unsigned)interrupt_isr29, 0x08, 0x8E);
    set_gate(30, (unsigned)interrupt_isr30, 0x08, 0x8E);
    set_gate(31, (unsigned)interrupt_isr31, 0x08, 0x8E);

    // Remap PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    // Setup IRQ's
    set_gate(32, (unsigned)interrupt_irq0, 0x08, 0x8E);
    set_gate(33, (unsigned)interrupt_irq1, 0x08, 0x8E);
    set_gate(34, (unsigned)interrupt_irq2, 0x08, 0x8E);
    set_gate(35, (unsigned)interrupt_irq3, 0x08, 0x8E);
    set_gate(36, (unsigned)interrupt_irq4, 0x08, 0x8E);
    set_gate(37, (unsigned)interrupt_irq5, 0x08, 0x8E);
    set_gate(38, (unsigned)interrupt_irq6, 0x08, 0x8E);
    set_gate(39, (unsigned)interrupt_irq7, 0x08, 0x8E);
    set_gate(40, (unsigned)interrupt_irq8, 0x08, 0x8E);
    set_gate(41, (unsigned)interrupt_irq9, 0x08, 0x8E);
    set_gate(42, (unsigned)interrupt_irq10, 0x08, 0x8E);
    set_gate(43, (unsigned)interrupt_irq11, 0x08, 0x8E);
    set_gate(44, (unsigned)interrupt_irq12, 0x08, 0x8E);
    set_gate(45, (unsigned)interrupt_irq13, 0x08, 0x8E);
    set_gate(46, (unsigned)interrupt_irq14, 0x08, 0x8E);
    set_gate(47, (unsigned)interrupt_irq15, 0x08, 0x8E);

    set_gate(0x80, (unsigned)interrupt_syscall, 0x08, 0x8E);

    x86_set_idt((uint32_t)&idtptr);
}

void x86_idt::set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    idt[num].kernel_cs = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags | 0x60;
}

void x86_idt::enable_interrupts() { asm volatile("sti"); }
void x86_idt::disable_interrupts() { asm volatile("cli"); }
