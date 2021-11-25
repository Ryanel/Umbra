#pragma once
#include <stdint.h>

struct gdt_entry_struct {
    uint16_t limit_low;    // The lower 16 bits of the limit.
    uint16_t base_low;     // The lower 16 bits of the base.
    uint8_t  base_middle;  // The next 8 bits of the base.
    uint8_t  access;       // Access flags, determine what ring this segment can be used in.
    uint8_t  granularity;
    uint8_t  base_high;  // The last 8 bits of the base.
} __attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct {
    uint16_t limit;  // The upper 16 bits of all selector limits.
    uint32_t base;   // The address of the first gdt_entry_t struct.
} __attribute__((packed));
typedef struct gdt_ptr_struct gdt_ptr_t;

void init_gdt();

typedef volatile struct strtss {
    unsigned short link;
    unsigned short link_h;
    unsigned long  esp0;
    unsigned short ss0;
    unsigned short ss0_h;
    unsigned long  esp1;
    unsigned short ss1;
    unsigned short ss1_h;
    unsigned long  esp2;
    unsigned short ss2;
    unsigned short ss2_h;
    unsigned long  cr3;
    unsigned long  eip;
    unsigned long  eflags;
    unsigned long  eax;
    unsigned long  ecx;
    unsigned long  edx;
    unsigned long  ebx;
    unsigned long  esp;
    unsigned long  ebp;
    unsigned long  esi;
    unsigned long  edi;
    unsigned short es;
    unsigned short es_h;
    unsigned short cs;
    unsigned short cs_h;
    unsigned short ss;
    unsigned short ss_h;
    unsigned short ds;
    unsigned short ds_h;
    unsigned short fs;
    unsigned short fs_h;
    unsigned short gs;
    unsigned short gs_h;
    unsigned short ldt;
    unsigned short ldt_h;
    unsigned short trap;
    unsigned short iomap;
} __attribute__((packed)) tss_struct;

extern tss_struct sys_tss;  // Define the TSS as a global structure