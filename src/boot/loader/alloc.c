#include <stdint.h>
#include <stddef.h>
#include <system.h>
#include <alloc.h>

#define DEBUG_MALLOC

#ifdef DEBUG_MALLOC
#include <stdio.h>
#include <panic.h>
#endif

int malloc_alignment = 0x1;

void* malloc(unsigned int sz) {
    // Allocate in malloc_alignment chunks
    uint32_t ret_addr = sys_config.mem_loader_cur;

    if(ret_addr + sz > sys_config.mem_loader_hi) {
        #ifdef DEBUG_MALLOC
        panic("Out of memory!");
        #endif
        return (void*)0; // Out of memory!
    }
    
    // Align the address
    if ((ret_addr % malloc_alignment) != 0) { 
        uint32_t new_addr = ret_addr + malloc_alignment - (ret_addr % malloc_alignment);
        //printf("loader  | DEBUG unaligned: @ 0x%x, new addr is 0x%x\n", ret_addr, new_addr);
        ret_addr = new_addr;
    }

    #ifdef DEBUG_MALLOC
    printf("loader  | alloc: size: 0x%x, returned: 0x%x, left: 0x%x\n", sz, ret_addr, sys_config.mem_loader_hi - sys_config.mem_loader_cur - sz);
    #endif

    sys_config.mem_loader_cur = ret_addr + sz;
    return (void*)ret_addr;
}

void malloc_set_alignment(uint32_t align) {
    malloc_alignment = align;
}