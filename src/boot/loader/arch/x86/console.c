#include <console.h>

void console_shim_write_char(unsigned int x, unsigned int y, char c, unsigned char attribute) {
    uint8_t * buffer = (uint8_t*)0xB8000;

    buffer[((y * 80) + x) * 2 ] = c;
    buffer[((y * 80) + x) * 2 + 1 ] = attribute;
}