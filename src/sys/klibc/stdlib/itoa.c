#include <stdlib.h>

char* itoa(unsigned long long n, unsigned int base) {
    const char  lut[] = "0123456789ABCDEF";
    static char buffer[sizeof(unsigned long) * 8 + 1];

    char* buf_index = &buffer[sizeof(buffer) - 1];
    *buf_index      = '\0';

    do {
        *--buf_index = lut[n % base];
        n /= base;
    } while (n > 0);
    return (char*)buf_index;
}
