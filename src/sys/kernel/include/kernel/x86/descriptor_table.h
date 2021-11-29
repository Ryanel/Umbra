#pragma once

#include <stdint.h>

typedef struct gdt_entry_struct {
    uint16_t limit_low;    // The lower 16 bits of the limit.
    uint16_t base_low;     // The lower 16 bits of the base.
    uint8_t  base_middle;  // The next 8 bits of the base.
    uint8_t  access;       // Access flags, determine what ring this segment can be used in.
    uint8_t  granularity;  //
    uint8_t  base_high;    // The last 8 bits of the base.
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt_ptr_struct {
    uint16_t limit;  // The upper 16 bits of all selector limits.
    uint32_t base;   // The address of the first gdt_entry_t struct.
} __attribute__((packed)) gdt_ptr_t;

typedef volatile struct strtss {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed)) tss_struct_t;

namespace kernel {
namespace x86 {

class gdt {
    gdt_ptr_t   gdt_ptr;
    gdt_entry_t gdt_entries[6];

   public:
    tss_struct_t tss;
    void         init();
    void         gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
    void         set_kernel_stack(uint32_t esp) { tss.esp0 = esp; }
};

extern gdt g_gdt;

}  // namespace x86
}  // namespace kernel
