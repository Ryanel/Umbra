#include <kernel/log.h>
#include <kernel/x86/descriptor_table.h>
#include <string.h>

extern "C" void* stack;
extern "C" void  setupGDT(uintptr_t);

void kernel::x86::gdt::init(unsigned int core) {
    this->entries[0] = {0x0000, 0x00, 0x00, 0x00, 0x00, 0x00};  // Null Entry
    this->entries[1] = {0xFFFF, 0x00, 0x00, 0x9A, 0xAF, 0x00};  // Kernel Code
    this->entries[2] = {0xFFFF, 0x00, 0x00, 0x92, 0xAF, 0x00};  // Kernel Data
    this->entries[3] = {0xFFFF, 0x00, 0x00, 0xFA, 0xAF, 0x00};  // User Code
    this->entries[4] = {0xFFFF, 0x00, 0x00, 0xF2, 0xAF, 0x00};  // User Data

    // Setup TSS
    uintptr_t tss_addr = (uintptr_t)&tss;
    this->entries[5]   = {sizeof(tss), (uint16_t)(tss_addr & 0xFFFF), (uint8_t)((tss_addr >> 16) & 0xFF), 0xE9,
                        0x00,        (tss_addr >> 24) && 0xFF};  // Core TSS
    tss_entry.base     = (tss_addr >> 32) & 0xFFFFFFFF;

    if (core == 0) { tss.rsp[0] = (uintptr_t)&stack + 8192; }

    pointer.limit = sizeof(entries) + sizeof(tss_entry) - 1;
    pointer.base  = (uintptr_t)&entries;
}

void kernel::x86::gdt::install() { setupGDT((uintptr_t)&pointer); }