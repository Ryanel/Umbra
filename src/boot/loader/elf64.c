#include <alloc.h>
#include <elf/elf64.h>
#include <panic.h>
#include <console.h>
#include <alloc.h>

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

    // We need to find the length of the memory image and copy from the file to a page-aligned destination.
    uint32_t mem_size = 0;

    for(unsigned int i = 0; i < header->e_phnum; i++) {  
        elf64_pheader_t * ph = (elf64_pheader_t*)(file->start + header->e_phoff + (header->e_phentsize * i));
        if(ph->p_type != 1) { return; }
        mem_size += ph->p_memsz;
    }

    // Allocate space and memcpy binary there
    malloc_set_alignment(0x1000);

    void* mem_start = malloc(mem_size);
    for(unsigned int i = 0; i < header->e_phnum; i++) {  
        elf64_pheader_t * ph = (elf64_pheader_t*)(file->start + header->e_phoff + (header->e_phentsize * i));
        if(ph->p_type != 1) { return; }
        printf("elfloader: LOAD %p bytes\n", ph->p_memsz);
        //memcpy(mem_start, (file->start + ph->p_offset), ph->p_filesz);
    }

    return result;
}