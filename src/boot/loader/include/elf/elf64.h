#pragma once

#include <system.h>

#define EI_NIDENT	16

// ELF Types
typedef uint64_t	Elf64_Addr;
typedef uint16_t	Elf64_Half;
typedef int16_t	Elf64_SHalf;
typedef uint64_t	Elf64_Off;
typedef int32_t	Elf64_Sword;
typedef uint32_t	Elf64_Word;
typedef uint64_t	Elf64_Xword;
typedef int64_t	Elf64_Sxword;


typedef struct elf_memory_image {
    uint64_t paddr_start, paddr_sz;
    uint64_t vaddr_start, vaddr_sz;
    uint64_t entry_address;
} elf_memory_image_t;

typedef struct elf64_header {
    unsigned char	e_ident[EI_NIDENT];
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off e_phoff;
    Elf64_Off e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} elf64_header_t;

elf_memory_image_t * elf64_load_file(system_file_t * file);