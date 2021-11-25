#include <kernel/x86/gdt.h>

gdt_entry_t gdt_entries[6];
gdt_ptr_t   gdt_ptr;
tss_struct  sys_tss;

extern "C" void setupGDT(uint32_t);

static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access = access;
}

void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                 // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);  // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);  // Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);  // User mode code segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);  // User mode data segment

    auto tss = (uint32_t)&sys_tss;
    gdt_set_gate(5, tss, tss + sizeof(tss_struct) + 1, 0x89, 0xCF);  // TSS

    setupGDT((uint32_t)&gdt_ptr);
}