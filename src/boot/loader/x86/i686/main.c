#include <console.h>
#include <i686/modules.h>
#include <i686/multiboot.h>
#include <panic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void* kernel_discover_info(multiboot_module_t* kernel);

void loader_main(uint32_t multiboot_magic, multiboot_info_t* multiboot_struct) {
    // Initialize the VGA console.
    console_init();

    printf("loader: Umbra %c-kernel loader for i686 initializing.\n", 230);

    // Ensure multiboot info is sane before we touch anything.
    if (multiboot_magic != 0x2BADB002) { panic("Multiboot magic is incorrect"); }

    // Extract data from multiboot
    multiboot_get_configuration();

    // Locate the loaded modules.
    multiboot_modules_t multiboot_modules = multiboot_init_modules(multiboot_struct);

    // Parse the kernel. (It's an ELF 32 kernel)
    kernel_discover_info(multiboot_modules.kernel);
    // kernel_info_t * kinfo = kernel_discover_info(multiboot_modules.kernel);

    // Allocate a region large enough to store the kernel image decompressed.
    // uintptr_t kernel_start_address = allocate_region(kinfo->mem_size, KERNEL_MEMORY_IMAGE, PAGE_ALIGNED);

    // Load the kernel into memory.

    // Initialise paging
    // paging_init();

    // size, physstart, virtstart
    // paging_map(0x1000000, 0x0, 0x0); // Memory Map the first megabyte.

    // paging_map(kinfo->mem_size, kernel_start_address, kinfo->virt_start); // Map kernel to proper virtual address at
    // 0xC00000000 paging_map(multiboot_modules.initrd->size, multiboot_modules.initrd->start, 0xD0000000); //Map initrd
    // to 0xD0000000 paging_map(0x1000000, 0x0, 0x0); //Map configuration page via identity map.

    // paging_commit();

    // Create configuration structure and store it in the configuration page. Pass this as a register to the kernel.

    // Boot into kernel!
    printf("loader: Starting kernel process...\n");
}