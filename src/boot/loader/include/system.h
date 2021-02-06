#pragma once

#include <stdint.h>

typedef struct system_config {
    int num_cores;
    int num_modules;
    uint32_t mem_loader_lo;
    uint32_t mem_loader_cur;
    uint32_t mem_loader_hi;
} system_config_t;

extern system_config_t sys_config;