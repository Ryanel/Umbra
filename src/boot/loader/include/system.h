#pragma once

#include <stdbool.h>
#include <stdint.h>

#define SYSTEM_MODULE_TYPE_UNKNOWN 0
#define SYSTEM_MODULE_TYPE_KERNEL  1
#define SYSTEM_MODULE_TYPE_CONFIG  2
#define SYSTEM_MODULE_TYPE_INITRD  3

typedef struct system_file {
    uint32_t            start;  ///< Start of a file
    uint32_t            size;   ///< Size of a file
    uint32_t            type;   ///< Type of file
    struct system_file* next;   ///< Next one in linked list
} system_file_t;

typedef struct system_config {
    uint32_t       num_cores;
    uint32_t       num_files;
    uint32_t       mem_loader_lo;
    uint32_t       mem_loader_cur;
    uint32_t       mem_loader_hi;
    system_file_t* boot_files;
    bool           framebuffer_enabled;
} system_config_t;

extern system_config_t sys_config;