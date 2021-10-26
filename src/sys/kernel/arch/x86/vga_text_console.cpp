#include <kernel/x86/ports.h>
#include <kernel/x86/vga_text_console.h>
#include <string.h>

using namespace kernel::device;

void vga_text_console::init() {}

void vga_text_console::clear() {
    char* buffer = (char*)(buffer_address);
    for (int i = 0; i < width() * height(); i++) {
        buffer[i * 2 + 0] = 0;
        buffer[i * 2 + 1] = this->attribute;
    }
}

void vga_text_console::write(char c) {
    char*        buffer = (char*)(buffer_address);
    unsigned int offset = index(x, y) * 2;
    switch (c) {
        case '\n':
            x = 0;
            y++;
            break;
        case '\b':
            x -= 1;
            if (x < 0) { x = 0; }
            offset         = index(x, y) * 2;
            buffer[offset] = ' ';
            break;
        default:
            buffer[offset]     = c;
            buffer[offset + 1] = this->attribute;
            x++;
            break;
    }
}

void vga_text_console::write_color(char c, char color) {
    char*        buffer = (char*)(buffer_address);
    unsigned int offset = index(x, y) * 2;
    switch (c) {
        case '\n':
            x = 0;
            y++;
            break;
        case '\b':
            x -= 1;
            if (x < 0) { x = 0; }
            offset         = index(x, y) * 2;
            buffer[offset] = ' ';
            break;
        default:
            buffer[offset]     = c;
            buffer[offset + 1] = color;
            x++;
            break;
    }
}

int  vga_text_console::width() { return 80; }
int  vga_text_console::height() { return 25; }
bool vga_text_console::supports_color() { return true; }
bool vga_text_console::supports_cursor_position() { return true; }
void vga_text_console::setX(int x) { this->x = x; }
void vga_text_console::setY(int y) { this->y = y; }
int  vga_text_console::getX() { return x; }
int  vga_text_console::getY() { return y; }
