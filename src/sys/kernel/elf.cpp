#include <kernel/elf.h>
#include <kernel/log.h>

bool kernel::elf_file::valid() const {
    if (m_header == nullptr) { return false; }

    if (m_header->e_ident[Elf_Ident::EI_MAG0] != 0x7F) { return false; }
    if (m_header->e_ident[Elf_Ident::EI_MAG1] != 'E') { return false; }
    if (m_header->e_ident[Elf_Ident::EI_MAG2] != 'L') { return false; }
    if (m_header->e_ident[Elf_Ident::EI_MAG3] != 'F') { return false; }
    if (m_header->e_ident[Elf_Ident::EI_VERSION] != 1) { return false; }
    if (m_header->e_ident[Elf_Ident::EI_DATA] != 1) { return false; }

    return true;
}

bool kernel::elf_file::loadable() const {
    // Check architecture here
    if (m_header->e_ident[Elf_Ident::EI_CLASS] != 1) { return false; }
    return m_header->e_machine == 0x3;  // x86
}

bool kernel::elf_file::executable() const {
    // Check type here
    return m_header->e_type == 0x2;  // Executable
}

void kernel::elf_file::debug_print() {
    kernel::log::debug("elf", "is: %s, %s, %s\n", valid() ? "valid" : "invalid", executable() ? "executable" : "not executable",
                       loadable() ? "loadable" : "not loadable");
    kernel::log::debug("elf", "entry: 0x%08x\n", m_header->e_entry);
    kernel::log::debug("elf", "%d sections\n", section_num());
    for (unsigned int i = 0; i < section_num(); i++) {
        auto* sec = section_header(i);
        kernel::log::debug("elf", "section %d: %s\n", i, sec->type_name());
    }

    for (unsigned int i = 0; i < prog_num(); i++) {
        auto* sec = prog_header(i);
        kernel::log::debug("elf", "ph %d: %s vaddr: 0x%08x, filesz: 0x%08x, memsz: 0x%08x\n", i, sec->type_name(), sec->p_vaddr,
                           sec->p_filesz, sec->p_memsz);
    }
}