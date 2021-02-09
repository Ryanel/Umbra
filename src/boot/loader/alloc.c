#include <stdint.h>
#include <stddef.h>
#include <system.h>
#include <alloc.h>

#define DEBUG_MALLOC

#ifdef DEBUG_MALLOC
#include <stdio.h>
#include <panic.h>
#endif

int malloc_alignment = 0x1000;

void* malloc(unsigned int sz) {
    // Allocate in malloc_alignment chunks
    uint32_t ret_addr = sys_config.mem_loader_cur;
    
    #ifdef DEBUG_MALLOC
    printf("loader  | alloc: size: 0x%x, returned: 0x%x, left: 0x%x\n", sz, sys_config.mem_loader_cur, sys_config.mem_loader_hi - sys_config.mem_loader_cur - sz);
    #endif

    if(ret_addr + sz > sys_config.mem_loader_hi) {
        #ifdef DEBUG_MALLOC
        panic("Out of memory!");
        #endif
        return (void*)0; // Out of memory!
    }


    // TODO: Allow for allocation alignment
    sys_config.mem_loader_cur += sz;
    return (void*)ret_addr;
}