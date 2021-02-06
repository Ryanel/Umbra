#include <console.h>
#include <stddef.h>
#include <stdint.h>

struct ConsoleData {
    unsigned int  x, y;
    unsigned char color;
    unsigned int  width, height;
    uint8_t*      buffer_addr;
};

struct ConsoleData console;

void console_init() {
    console.x           = 0;
    console.y           = 0;
    console.width       = 80;
    console.height      = 25;
    console.color       = CONSOLE_DEFAULT_COLOR;
    console.buffer_addr = (uint8_t*)0xB8000;
}

void console_scroll() {
    for (unsigned int line = 0; line < (console.height - 1); line++) {
        for (unsigned int x = 0; x < console.width; x++) {
            unsigned int src  = 2 * (((line + 1) * console.width) + x);
            unsigned int dest = 2 * ((line * console.width) + x);

            char c   = console.buffer_addr[src];
            char att = console.buffer_addr[src + 1];

            console.buffer_addr[dest]     = c;
            console.buffer_addr[dest + 1] = att;
        }
    }
    // Clear last line
    for (unsigned int x = 0; x < console.width; x++) { 
        console_shim_write_char(x, console.height - 1, ' ', console.color);
    }
}

void console_newline() {
    console.y += 1;
    console.x = 0;

    if (console.y >= console.height) {
        console.y = console.height - 1;
        console_scroll();
    }
}

void console_print_char(char c) {
    // Filter newlines
    if (c == '\n') {
        console_newline();
    } else {
        console_shim_write_char(console.x, console.y, c, console.color);
        console.x++;
        if (console.x >= console.width) { console_newline(); }
    }
}

void console_print_string(const char * s) {
    int i = 0;
    while (s[i] != '\0') {
        console_print_char(s[i]);
        i++;
    }
}

void console_clear() {
    for (unsigned int x = 0; x < console.width; x++) {
        for (unsigned int y = 0; y < console.height; y++) {
            console_shim_write_char(x, y, ' ', CONSOLE_DEFAULT_COLOR);
        }
    }
}