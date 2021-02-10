#include <alloc.h>
#include <elf/elf64.h>
#include <panic.h>
elf_memory_image_t* elf64_load_file(system_file_t* file) {
    elf_memory_image_t* result = malloc(sizeof(elf_memory_image_t));

    // Load and check the ELF header
    elf64_header_t* header = (elf64_header_t*)(file->start);

    if(file->type != SYSTEM_MODULE_TYPE_KERNEL) {
        panic("Loading a non-kernel is not supported at this time.");
    }

    if (header->e_ident[0] != 0x7F || header->e_ident[1] != 'E' || header->e_ident[2] != 'L' || header->e_ident[3] != 'F') {
        panic("File is not an ELF image");
    }

    if(header->e_ident[4] != 2) {
        panic("ELF image is not 64-bit");
    }

    return result;
}