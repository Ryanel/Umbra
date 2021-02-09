#pragma once

#include <stdint.h>

void* malloc(unsigned int sz);
void malloc_set_alignment(uint32_t align);