#include <string.h>

char* strncpy(char* dest, const char* src, size_t count) {
    unsigned char* destPtr = (unsigned char*)dest;
    unsigned char* srcPtr  = (unsigned char*)src;

    int write_null = 0;

    for (size_t i = 0; i < count; i++) {
        if (!write_null) {
            *destPtr = *srcPtr;
        } else {
            *destPtr = 0;
        }

        if (*srcPtr == 0) { write_null = 1; }

        destPtr++;
        srcPtr++;
    }

    return dest;
}
