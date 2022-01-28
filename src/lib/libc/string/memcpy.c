#include <stdint.h>
#include <string.h>

void* memcpy(void* dest, const void* src, size_t n) {
    char* src_c  = (char*)src;
    char* dest_c = (char*)dest;

    for (size_t i = 0; i < n; i++) { dest_c[i] = src_c[i]; }
    return dest;
}