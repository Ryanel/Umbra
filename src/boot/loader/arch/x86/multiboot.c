#include <alloc.h>
#include <console.h>
#include <i686/modules.h>
#include <i686/multiboot.h>
#include <panic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <system.h>

void multiboot_get_configuration(multiboot_info_t* mbs) {
    sys_config.num_cores = 1;
    sys_config.num_files = mbs->mods_count;

    // Initalise the console. TODO: Check for framebuffer!
    sys_config.framebuffer_enabled = false;
    console_init();

    // Parse memory map for allocatable areas.
    // If the memory flag not set, panic!
    if ((mbs->flags & 0x01) == 0) { panic("Multiboot loader did not populate memory information"); }

    uint32_t loader_memory_lo = 0x1000;  // Set to 0x1000 so we never use the zero-page.
    uint32_t loader_memory_hi = loader_memory_lo;

    multiboot_memory_map_t* mmap_entry = (multiboot_memory_map_t*)mbs->mmap_addr;
    while ((multiboot_uint32_t)mmap_entry < mbs->mmap_addr + mbs->mmap_length) {
        // Locating the memory region able to be used by the loader for dynamic allocation.
        if (mmap_entry->type == 1 && mmap_entry->addr < 0x100000) {
            loader_memory_hi = mmap_entry->addr + mmap_entry->len;
            if (loader_memory_lo < mmap_entry->addr) { loader_memory_lo = mmap_entry->addr; }
        }
        // Advance entry
        mmap_entry = (multiboot_memory_map_t*)((unsigned int)mmap_entry + mmap_entry->size + sizeof(mmap_entry->size));
    }

    // Dynamic memory allocation can now be used!
    sys_config.mem_loader_lo  = loader_memory_lo;
    sys_config.mem_loader_hi  = loader_memory_hi;
    sys_config.mem_loader_cur = loader_memory_lo;

    // Now, allocate the memory map to system config.

    // Print some debug messages
    printf("loader: Alloc using 0x%08x => 0x%08x\n", loader_memory_lo, loader_memory_hi);

    // Now, scan for modules and create system_module_t entries for them.
    system_file_t* root_module = NULL;
    system_file_t* cur_module  = NULL;
    sys_config.boot_files      = NULL;

    for (size_t i = 0; i < mbs->mods_count; i++) {
        if (i == 0) {
            cur_module            = (system_file_t*)malloc(sizeof(system_file_t));
            root_module           = cur_module;
            sys_config.boot_files = root_module;
        } else {
            system_file_t* entry = (system_file_t*)malloc(sizeof(system_file_t));
            cur_module->next     = entry;
            cur_module           = entry;
        }

        multiboot_module_t* mul_mod = (multiboot_module_t*)(mbs->mods_addr + (16 * i));
        cur_module->next            = NULL;
        cur_module->start           = mul_mod->mod_start;
        cur_module->size            = mul_mod->mod_end - mul_mod->mod_start;

        if (strcmp((const char*)mul_mod->cmdline, "kernel") == 0) {
            cur_module->type = SYSTEM_MODULE_TYPE_KERNEL;
        } else if (strcmp((const char*)mul_mod->cmdline, "config") == 0) {
            cur_module->type = SYSTEM_MODULE_TYPE_CONFIG;
        } else if (strcmp((const char*)mul_mod->cmdline, "initrd") == 0) {
            cur_module->type = SYSTEM_MODULE_TYPE_INITRD;
        } else {
            cur_module->type = SYSTEM_MODULE_TYPE_UNKNOWN;
        }

        printf("loader: Found module @ %08x -> %08x \"%s\" (%d)\n", cur_module->start, cur_module->start + cur_module->size,
               (char*)mul_mod->cmdline, cur_module->type);
    }

    system_file_t* rm = root_module;
    while (rm != NULL) { rm = rm->next; }
}