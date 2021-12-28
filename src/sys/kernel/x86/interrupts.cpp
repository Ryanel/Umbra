#define DEF_INT_HANDLERS

#include <kernel/interrupts.h>
#include <kernel/log.h>
#include <kernel/mm/vmm.h>
#include <kernel/panic.h>
#include <kernel/time.h>
#include <kernel/x86/interrupts.h>
#include <kernel/x86/ports.h>
#include <string.h>

void interrupts_disable() { asm("cli"); }
void interrupts_enable() { asm("sti"); }
void interrupts_after_thread() { outb(0x20, 0x20); }

extern "C" void k_exception_handler(register_frame_t* regs) {
    if (kernel::interrupts::dispatch(regs->int_no, regs)) {
        // We've handled and returned from this exception. Let the PIT know and we'll return to the process
        if (regs->int_no >= 40) { outb(0xA0, 0x20); }
        outb(0x20, 0x20);
        return;
    }

#ifdef ARCH_X86

    kernel::log::error("error", "eip: 0x%08x int:%02x err:%08x eflags:%08x\n", regs->eip, regs->int_no, regs->err_code,
                       regs->eflags);
    kernel::log::error("error", "cs:%02x ds:%02x es:0x%02x fs:%02x gs:%02x\n", regs->cs, regs->ds, regs->es, regs->fs,
                       regs->gs);
    kernel::log::error("error", "eax:%08x ebx:%08x ecx:%08x edx:%08x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
    kernel::log::error("error", "ebp:%08x esp:%08x esi:%08x edi:%08x\n", regs->edi, regs->esi, regs->esp, regs->ebp);

#else

    kernel::log::error("error", "rip: 0x%016p int:%02x err:%08x rflags:%08x\n", regs->rip, regs->int_no, regs->err_code,
                       regs->eflags);
    kernel::log::error("error", "rax:%016p rbx:%016p rcx:%016p rdx:%016p\n", regs->rax, regs->rbx, regs->rcx, regs->rdx);
    kernel::log::error("error", "rdi:%016p rsi:%016p rbp:%016p\n", regs->rdi, regs->rsi, regs->rbp);

#endif
    panic("Unhandled exception");
}
extern "C" void k_irq_handler(register_frame_t* regs) {
    kernel::interrupts::dispatch(regs->int_no, regs);
    if (regs->int_no >= 40) { outb(0xA0, 0x20); }
    outb(0x20, 0x20);
}

void x86_idt::init() {
    idtptr.limit = (uint16_t)sizeof(idt_entry_t) * 256 - 1;
    idtptr.base  = (uintptr_t)&idt[0];

    constexpr int descriptor = 0x08;

    memset(&idt, 0, sizeof(idt_entry_t) * 256);
    set_gate(0, (uintptr_t)interrupt_isr0, descriptor, 0x8E);
    set_gate(1, (uintptr_t)interrupt_isr1, descriptor, 0x8E);
    set_gate(2, (uintptr_t)interrupt_isr2, descriptor, 0x8E);
    set_gate(3, (uintptr_t)interrupt_isr3, descriptor, 0x8E);
    set_gate(4, (uintptr_t)interrupt_isr4, descriptor, 0x8E);
    set_gate(5, (uintptr_t)interrupt_isr5, descriptor, 0x8E);
    set_gate(6, (uintptr_t)interrupt_isr6, descriptor, 0x8E);
    set_gate(7, (uintptr_t)interrupt_isr7, descriptor, 0x8E);

    set_gate(8, (uintptr_t)interrupt_isr8, descriptor, 0x8E);
    set_gate(9, (uintptr_t)interrupt_isr9, descriptor, 0x8E);
    set_gate(10, (uintptr_t)interrupt_isr10, descriptor, 0x8E);
    set_gate(11, (uintptr_t)interrupt_isr11, descriptor, 0x8E);
    set_gate(12, (uintptr_t)interrupt_isr12, descriptor, 0x8E);
    set_gate(13, (uintptr_t)interrupt_isr13, descriptor, 0x8E);
    set_gate(14, (uintptr_t)interrupt_isr14, descriptor, 0x8E);
    set_gate(15, (uintptr_t)interrupt_isr15, descriptor, 0x8E);

    set_gate(16, (uintptr_t)interrupt_isr16, descriptor, 0x8E);
    set_gate(17, (uintptr_t)interrupt_isr17, descriptor, 0x8E);
    set_gate(18, (uintptr_t)interrupt_isr18, descriptor, 0x8E);
    set_gate(19, (uintptr_t)interrupt_isr19, descriptor, 0x8E);
    set_gate(20, (uintptr_t)interrupt_isr20, descriptor, 0x8E);
    set_gate(21, (uintptr_t)interrupt_isr21, descriptor, 0x8E);
    set_gate(22, (uintptr_t)interrupt_isr22, descriptor, 0x8E);
    set_gate(23, (uintptr_t)interrupt_isr23, descriptor, 0x8E);

    set_gate(24, (uintptr_t)interrupt_isr24, descriptor, 0x8E);
    set_gate(25, (uintptr_t)interrupt_isr25, descriptor, 0x8E);
    set_gate(26, (uintptr_t)interrupt_isr26, descriptor, 0x8E);
    set_gate(27, (uintptr_t)interrupt_isr27, descriptor, 0x8E);
    set_gate(28, (uintptr_t)interrupt_isr28, descriptor, 0x8E);
    set_gate(29, (uintptr_t)interrupt_isr29, descriptor, 0x8E);
    set_gate(30, (uintptr_t)interrupt_isr30, descriptor, 0x8E);
    set_gate(31, (uintptr_t)interrupt_isr31, descriptor, 0x8E);

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
    set_gate(32, (uintptr_t)interrupt_irq0, descriptor, 0x8E);
    set_gate(33, (uintptr_t)interrupt_irq1, descriptor, 0x8E);
    set_gate(34, (uintptr_t)interrupt_irq2, descriptor, 0x8E);
    set_gate(35, (uintptr_t)interrupt_irq3, descriptor, 0x8E);
    set_gate(36, (uintptr_t)interrupt_irq4, descriptor, 0x8E);
    set_gate(37, (uintptr_t)interrupt_irq5, descriptor, 0x8E);
    set_gate(38, (uintptr_t)interrupt_irq6, descriptor, 0x8E);
    set_gate(39, (uintptr_t)interrupt_irq7, descriptor, 0x8E);
    set_gate(40, (uintptr_t)interrupt_irq8, descriptor, 0x8E);
    set_gate(41, (uintptr_t)interrupt_irq9, descriptor, 0x8E);
    set_gate(42, (uintptr_t)interrupt_irq10, descriptor, 0x8E);
    set_gate(43, (uintptr_t)interrupt_irq11, descriptor, 0x8E);
    set_gate(44, (uintptr_t)interrupt_irq12, descriptor, 0x8E);
    set_gate(45, (uintptr_t)interrupt_irq13, descriptor, 0x8E);
    set_gate(46, (uintptr_t)interrupt_irq14, descriptor, 0x8E);
    set_gate(47, (uintptr_t)interrupt_irq15, descriptor, 0x8E);

    set_gate(0x80, (uintptr_t)interrupt_syscall, descriptor, 0x8E);

    kernel::log::debug("gdt", "Installing IDT");
    __asm__ volatile("lidt %0" : : "m"(idtptr));  // load the new IDT
    if (kernel::log::will_log(1)) { kernel::log::status_log("Done"); }
    kernel::interrupts::init();
}

void x86_idt::set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
#ifdef ARCH_X86
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (uint16_t)((base >> 16) & 0xFFFF);
#else
    idt[num].base_lo  = (uint64_t)base & 0xFFFF;
    idt[num].base_mid = ((uint64_t)base >> 16) & 0xFFFF;
    idt[num].base_hi  = ((uint64_t)base >> 32) & 0xFFFFFFFF;
    idt[num].ist      = 0;  // Unused for now
#endif
    idt[num].flags       = flags | 0x60;
    idt[num].selector    = sel;
    idt[num].always_zero = 0;
}

void x86_idt::enable_interrupts() { asm volatile("sti"); }
void x86_idt::disable_interrupts() { asm volatile("cli"); }
