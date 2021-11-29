#pragma once

#include <kernel/x86/regs.h>
#include <stdint.h>

#define IDT_ENTRY_ATTR_PRESENT   0x80
#define IDT_ENTRY_ATTR_DPL0      0x00 // Unsure if this is valid...
#define IDT_ENTRY_ATTR_DPL3      0x60
#define IDT_ENTRY_ATTR_INTERRUPT 0x05

/// IDT Entry
typedef struct idt_entry {
    uint16_t base_lo;
    uint16_t kernel_cs;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_hi;
} __attribute__((__packed__)) idt_entry_t;

/// Pointer to IDT
typedef struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((__packed__)) idt_ptr_t;

class x86_idt {
   public:
    void init();
    void set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
    void enable_interrupts();
    void disable_interrupts();
   private:
   __attribute__((aligned(0x10))) 
    idt_entry_t idt[256];
    idt_ptr_t   idtptr;
};

extern "C" void x86_set_idt(uint32_t idtr);

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

#ifdef DEF_INT_HANDLERS
extern "C" void interrupt_isr0();
extern "C" void interrupt_isr1();
extern "C" void interrupt_isr2();
extern "C" void interrupt_isr3();
extern "C" void interrupt_isr4();
extern "C" void interrupt_isr5();
extern "C" void interrupt_isr6();
extern "C" void interrupt_isr7();
extern "C" void interrupt_isr8();
extern "C" void interrupt_isr9();
extern "C" void interrupt_isr10();
extern "C" void interrupt_isr11();
extern "C" void interrupt_isr12();
extern "C" void interrupt_isr13();
extern "C" void interrupt_isr14();
extern "C" void interrupt_isr15();
extern "C" void interrupt_isr16();
extern "C" void interrupt_isr17();
extern "C" void interrupt_isr18();
extern "C" void interrupt_isr19();
extern "C" void interrupt_isr20();
extern "C" void interrupt_isr21();
extern "C" void interrupt_isr22();
extern "C" void interrupt_isr23();
extern "C" void interrupt_isr24();
extern "C" void interrupt_isr25();
extern "C" void interrupt_isr26();
extern "C" void interrupt_isr27();
extern "C" void interrupt_isr28();
extern "C" void interrupt_isr29();
extern "C" void interrupt_isr30();
extern "C" void interrupt_isr31();

extern "C" void interrupt_irq0();
extern "C" void interrupt_irq1();
extern "C" void interrupt_irq2();
extern "C" void interrupt_irq3();
extern "C" void interrupt_irq4();
extern "C" void interrupt_irq5();
extern "C" void interrupt_irq6();
extern "C" void interrupt_irq7();
extern "C" void interrupt_irq8();
extern "C" void interrupt_irq9();
extern "C" void interrupt_irq10();
extern "C" void interrupt_irq11();
extern "C" void interrupt_irq12();
extern "C" void interrupt_irq13();
extern "C" void interrupt_irq14();
extern "C" void interrupt_irq15();

extern "C" void interrupt_syscall();

#endif
