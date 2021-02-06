#include <i686/multiboot.h>
#include <i686/modules.h>
#include <stdint.h>
#include <system.h>
#include <stdio.h>
#include <panic.h>

void multiboot_get_configuration( multiboot_info_t* mbs) {
    printf("loader: Reading configuration from multiboot\n");
    sys_config.num_cores = 1;
    sys_config.num_modules = mbs->mods_count;
    
    // Parse memory map.
    // If the memory flag not set, panic!
    if((mbs->flags & 0x01) == 0) {
        panic("Multiboot loader did not populate memory information");
    }
    
    uint32_t loader_memory_lo = 0x1000; // Set to 0x1000 so we never use the zero-page.
    uint32_t loader_memory_hi = loader_memory_lo;

    multiboot_memory_map_t * mmap_entry = (multiboot_memory_map_t *)mbs->mmap_addr;
    while((multiboot_uint32_t)mmap_entry < mbs->mmap_addr + mbs->mmap_length) {
        // Locatingn the memory region able to be used by the loader for dynamic allocation.
        if(mmap_entry->type == 1) {
            if(mmap_entry->addr < 0x100000) {
                loader_memory_hi = mmap_entry->addr + mmap_entry->len;
                if(loader_memory_lo < mmap_entry->addr) {
                    loader_memory_lo = mmap_entry->addr;
                }
            }    
        }
        // Advance entry
        mmap_entry = (multiboot_memory_map_t *)((unsigned int)mmap_entry + mmap_entry->size + sizeof(mmap_entry->size));
    }

    // Dynamic memory allocation can now be used!
    sys_config.mem_loader_lo = loader_memory_lo;
    sys_config.mem_loader_hi = loader_memory_hi;
    sys_config.mem_loader_cur = loader_memory_lo;
    printf("loader: Dynamic Allocation able to use from 0x%08x to 0x%08x\n", loader_memory_lo, loader_memory_hi);
}