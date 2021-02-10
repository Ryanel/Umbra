#pragma once

#include <system.h>

typedef struct elf_memory_image {
    uint64_t paddr_start, paddr_sz;
    uint64_t vaddr_start, paddr_sz;
    uint64_t entry_address;
} elf_memory_image_t;

elf_memory_image_t * elf64_load_file(system_file_t * file);