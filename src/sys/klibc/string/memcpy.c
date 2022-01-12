#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Inspired by https://stackoverflow.com/questions/51494787/optimization-in-memcpy-implementation
void* memcpy(void* dest, const void* src, size_t len) {
    // Optimized version of memcpy
    // Copys blocks of 4 * ptrsize IF src and dest are aligned, and copying enough bytes.
    unsigned char*       ptr_dest = (unsigned char*)dest;
    unsigned char const* ptr_src  = (unsigned char const*)src;

    const size_t align    = sizeof(long);
    const size_t min_size = align * 2;
    const bool   aligned  = ((uintptr_t)src & (align - 1)) == ((uintptr_t)dest & (align - 1));

    if (len >= min_size && aligned) {
        while (((uintptr_t)ptr_src & (align - 1)) != 0) {
            *ptr_dest++ = *ptr_src++;
            len--;
        }
        long*       long_dest = (long*)ptr_dest;
        long const* long_src = (long const*)ptr_src;

        // Manually unrolled loop to copy 4 words
        while (len >= align * 4) {
            long_dest[0] = long_src[0];
            long_dest[1] = long_src[1];
            long_dest[2] = long_src[2];
            long_dest[3] = long_src[3];
            long_src += 4;
            long_dest += 4;
            len -= align * 4;
        }

        // Copy the rest of the words
        while (len >= align) {
            *long_dest++ = *long_src++;
            len -= align;
        }

        ptr_dest = (unsigned char*)long_dest;
        ptr_src  = (unsigned char const*)long_src;
    }
    // Copy the rest of the bytes
    while (len--) { *ptr_dest++ = *ptr_src++; }
    return dest;
}