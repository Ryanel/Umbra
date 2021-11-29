#include <kernel/x86/descriptor_table.h>
#include <string.h>
extern "C" void setupGDT(uint32_t);

kernel::x86::gdt kernel::x86::g_gdt;

void kernel::x86::gdt::gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access = access;
}

void kernel::x86::gdt::init() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                 // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);  // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);  // Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);  // User mode code segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);  // User mode data segment

    auto tss_ptr = (uint32_t)&tss;
    gdt_set_gate(5, tss_ptr, tss_ptr + sizeof(tss_struct_t), 0x89, 0xCF);  // TSS

    tss.ss0  = 0x10;
    tss.esp0 = 0x0;
    tss.cs   = 0x0b;
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x13;
    tss.iomap_base                             = sizeof(tss_struct_t);

    setupGDT((uint32_t)&gdt_ptr);
}

extern "C" void set_kernel_stack(uint32_t stk) {
    kernel::x86::g_gdt.set_kernel_stack(stk);
}