#define DEF_INT_HANDLERS

#include <kernel/log.h>
#include <kernel/x86/interrupts.h>
#include <kernel/x86/ports.h>
#include <string.h>

extern "C" void k_exception_handler(register_frame_t* regs) {}
extern "C" void k_irq_handler(register_frame_t* regs) {
    klogf("irq", "Recieved %x\n", regs->int_no - 32);

    // Signal interrupt handled
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

    x86_set_idt((uint32_t)&idtptr);
}

void x86_idt::set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    idt[num].kernel_cs = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

void x86_idt::enable_interrupts() { asm volatile("sti"); }
void x86_idt::disable_interrupts() { asm volatile("cli"); }