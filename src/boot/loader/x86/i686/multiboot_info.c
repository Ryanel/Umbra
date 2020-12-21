#include <i686/modules.h>
#include <i686/multiboot.h>
#include <panic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

multiboot_module_t* multiboot_find_module(char* search, multiboot_info_t* multiboot_struct) {
    uintptr_t module_struct_address = (uintptr_t)(multiboot_struct->mods_addr);
    uintptr_t module_struct_num     = (uintptr_t)(multiboot_struct->mods_count);
    // Search each module for search in the command line
    for (int i = 0; i < module_struct_num; i++) {
        multiboot_module_t* mod = (multiboot_module_t*)(module_struct_address + (sizeof(multiboot_module_t) * i));
        if (strcmp((const char*)mod->cmdline, search) == 0) { return mod; }
    }

    printf("loader: Unable to find %s in loaded multiboot files.\n", search);
    return 0;
}

multiboot_modules_t multiboot_init_modules(multiboot_info_t* multiboot_struct) {
    multiboot_modules_t modules;
    if (multiboot_struct->mods_count < 1) {
        panic("Kernel was not passed as a module to the loader.");
        modules.num = 0;
        return modules;
    }

    bool hasInitrd = multiboot_struct->mods_count > 1;

    multiboot_module_t* mod_kernel = multiboot_find_module("kernel", multiboot_struct);
    printf("loader: Kernel module found @ 0x%08x\n", mod_kernel);

    multiboot_module_t* mod_initrd = 0;
    if (hasInitrd) {
        mod_initrd     = multiboot_find_module("initrd", multiboot_struct);
        modules.initrd = mod_initrd;

        printf("loader: Initrd module found @ 0x%08x\n", mod_initrd);
    }

    modules.num        = multiboot_struct->mods_count;
    modules.has_initrd = hasInitrd ? 1 : 0;
    modules.kernel     = mod_kernel;

    return modules;
}

void multiboot_get_configuration() {}