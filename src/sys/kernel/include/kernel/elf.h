#pragma once

#include <stdint.h>
#define ELF_NIDENT 16

typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Word;
typedef int32_t  Elf32_Sword;

typedef struct {
    uint8_t    e_ident[ELF_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off  e_phoff;
    Elf32_Off  e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} elf32_header;

/// An ELF file meant to be loaded by the program loader.
class elf_file {
   public:
    bool valid() const;       //< Determines the integrity of the file
    bool loadable() const;    //< Determines if this is natively loadable by this operating system
    bool executable() const;  //< Is this file runnable as a task?

    elf_file(void* header) : m_header((elf32_header*)header) {}

   private:
    elf32_header* m_header;
};