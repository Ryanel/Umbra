#pragma once

#include <i686/multiboot.h>

typedef struct {
    int                 num;
    int                 has_initrd;
    multiboot_module_t* kernel;
    multiboot_module_t* initrd;
} multiboot_modules_t;

multiboot_module_t* multiboot_find_module(char* search, multiboot_info_t* multiboot_struct);
multiboot_modules_t multiboot_init_modules(multiboot_info_t* multiboot_struct);
void                multiboot_get_configuration(multiboot_info_t* multiboot_struct);