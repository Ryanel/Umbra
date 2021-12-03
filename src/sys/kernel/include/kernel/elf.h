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

enum Elf_Ident : unsigned int {
    EI_MAG0       = 0,  // 0x7F
    EI_MAG1       = 1,  // 'E'
    EI_MAG2       = 2,  // 'L'
    EI_MAG3       = 3,  // 'F'
    EI_CLASS      = 4,  // Architecture (32/64)
    EI_DATA       = 5,  // Byte Order
    EI_VERSION    = 6,  // ELF Version
    EI_OSABI      = 7,  // OS Specific
    EI_ABIVERSION = 8,  // OS Specific
    EI_PAD        = 9   // Padding
};

enum ShT_Types {
    SHT_NULL     = 0,  // Null section
    SHT_PROGBITS = 1,  // Program information
    SHT_SYMTAB   = 2,  // Symbol table
    SHT_STRTAB   = 3,  // String table
    SHT_RELA     = 4,  // Relocation (w/ addend)
    SHT_NOBITS   = 8,  // Not present in file
    SHT_REL      = 9,  // Relocation (no addend)
};

enum ShT_Attributes {
    SHF_WRITE = 0x01,  // Writable section
    SHF_ALLOC = 0x02   // Exists in memory
};

typedef struct {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off  sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;

    const char* type_name() const {
        switch (sh_type) {
            case SHT_NULL: return "Null";
            case SHT_PROGBITS: return "Program";
            case SHT_SYMTAB: return "Symbol Table";
            case SHT_STRTAB: return "String Table";
            case SHT_RELA: return "Relocation (Addend)";
            case SHT_NOBITS: return "Not Present";
            case SHT_REL: return "Relocation (No addend)";
            default: return "Unknown";
        }
    }
} elf32_section_t;

typedef struct {
    Elf32_Word p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;

    const char* type_name() const {
        switch (p_type) {
            case 0x1: return "LOAD";
            default: return "Unknown";
        }
    }
} elf32_prog_t;

/// An ELF file meant to be loaded by the program loader.
namespace kernel {
class elf_file {
   public:
    bool valid() const;       //< Determines the integrity of the file
    bool loadable() const;    //< Determines if this is natively loadable by this operating system
    bool executable() const;  //< Is this file runnable as a task?
    void debug_print();
    elf_file(void* header) : m_header((elf32_header*)header) {}

    inline unsigned int section_num() const { return m_header->e_shnum; }
    inline unsigned int prog_num() const { return m_header->e_phnum; }

    elf32_section_t* section_header(unsigned int index) {
        uintptr_t base = (uintptr_t)m_header + m_header->e_shoff;
        return (elf32_section_t*)(base + (sizeof(elf32_section_t) * index));
    }

    elf32_prog_t* prog_header(unsigned int index) {
        uintptr_t base = (uintptr_t)m_header + m_header->e_phoff;
        return (elf32_prog_t*)(base + (sizeof(elf32_prog_t) * index));
    }

    elf32_header* m_header;

   private:
};
}  // namespace kernel