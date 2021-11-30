#include <kernel/x86/ports.h>
#include <kernel/x86/vga_text_console.h>
#include <string.h>

using namespace kernel::device;

void vga_text_console::init() {
    x = 0;
    y = 0;

    // Set block cursor
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | 0);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
    clear(current_background); 
}

void vga_text_console::clear(unsigned char bg) {
    char* buffer       = (char*)(buffer_address);
    current_background = bg;

    uint8_t attribute = ((bg & 0xF) << 4) | 0xF;

    for (int i = 0; i < width() * height(); i++) {
        buffer[i * 2 + 0] = 0;
        buffer[i * 2 + 1] = attribute;
    }
}

void vga_text_console::write(char c, unsigned char fore, unsigned char back) {
    char*        buffer = (char*)(buffer_address);
    unsigned int offset = index(x, y) * 2;

    uint8_t attribute = ((back & 0xF) << 4) | (fore & 0xF);

    current_background = back;

    switch (c) {
        case '\n':
            x = 0;
            y++;
            scroll_up();
            break;
        case '\b':
            x--;
            if (x < 0) { x = 0; }
            offset         = index(x, y) * 2;
            buffer[offset] = ' ';
            break;
        default:
            buffer[offset]     = c;
            buffer[offset + 1] = attribute;
            x++;
            break;
    }

    if (x >= width()) {
        x = 0;
        y++;
        scroll_up();
    }
    set_cursor();
}

void vga_text_console::scroll_up() {
    char*     buffer = (char*)(buffer_address);
    const int sz     = width() * 2;
    if (y >= height()) {
        for (size_t i = 1; i < (size_t)height(); i++) { memcpy(&buffer[sz * (i - 1)], &buffer[sz * i], sz); }

        size_t start = width() * (height() - 1) * 2;

        for (int i = 0; i < width(); i++) {
            buffer[start + (i * 2) + 0] = ' ';
            buffer[start + (i * 2) + 1] = (current_background << 4);
        }

        y--;
    }
}

void vga_text_console::set_cursor() {
    uint16_t pos = y * width() + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

int  vga_text_console::width() { return 80; }
int  vga_text_console::height() { return 25; }
bool vga_text_console::supports_cursor_position() { return true; }
void vga_text_console::setX(int x) { this->x = x; }
void vga_text_console::setY(int y) { this->y = y; }
int  vga_text_console::getX() { return x; }
int  vga_text_console::getY() { return y; }
