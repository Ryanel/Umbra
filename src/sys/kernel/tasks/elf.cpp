#include <kernel/log.h>
#include <kernel/tasks/elf.h>

bool kernel::elf32_file::valid() const {
    if (m_header == nullptr) { return false; }

    if (m_header->e_ident[Elf_Ident::EI_MAG0] != 0x7F) { return false; }
    if (m_header->e_ident[Elf_Ident::EI_MAG1] != 'E') { return false; }
    if (m_header->e_ident[Elf_Ident::EI_MAG2] != 'L') { return false; }
    if (m_header->e_ident[Elf_Ident::EI_MAG3] != 'F') { return false; }
    if (m_header->e_ident[Elf_Ident::EI_VERSION] != 1) { return false; }
    if (m_header->e_ident[Elf_Ident::EI_DATA] != 1) { return false; }

    return true;
}

bool kernel::elf32_file::loadable() const {
    // Check architecture here
    if (m_header->e_ident[Elf_Ident::EI_CLASS] != 1) { return false; }
    return m_header->e_machine == 0x3;  // x86
}

bool kernel::elf32_file::executable() const {
    // Check type here
    return m_header->e_type == 0x2;  // Executable
}

// 64-bit

bool kernel::elf64_file::valid() const {
    if (m_header == nullptr) { return false; }

    if (m_header->e_ident[Elf_Ident::EI_MAG0] != 0x7F) { return false; }
    if (m_header->e_ident[Elf_Ident::EI_MAG1] != 'E') { return false; }
    if (m_header->e_ident[Elf_Ident::EI_MAG2] != 'L') { return false; }
    if (m_header->e_ident[Elf_Ident::EI_MAG3] != 'F') { return false; }
    if (m_header->e_ident[Elf_Ident::EI_VERSION] != 1) { return false; }
    if (m_header->e_ident[Elf_Ident::EI_DATA] != 1) { return false; }

    return true;
}

bool kernel::elf64_file::loadable() const {
    // Check architecture here
    if (m_header->e_ident[Elf_Ident::EI_CLASS] != 1) { return false; }
    return m_header->e_machine == 0x3E;  // x86
}

bool kernel::elf64_file::executable() const {
    // Check type here
    return m_header->e_type == 0x2;  // Executable
}
