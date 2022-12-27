#include <kernel/x86/ports.h>
#include <kernel/x86/vga_text_console.h>
#include <string.h>

using namespace kernel::device;

void vga_text_console::init() {
    m_x = 0;
    m_y = 0;

    // Set block cursor
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | 0);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
    clear(current_background);
    m_buffer = util::buffer<unsigned char>((unsigned char*)buffer_address, width() * height() * 2);
}

void vga_text_console::clear(unsigned char bg) {
    current_background = bg;

    uint8_t attribute = (uint8_t)((bg & 0xF) << 4) | 0xF;

    for (int i = 0; i < width() * height(); i++) {
        m_buffer[i * 2 + 0] = 0;
        m_buffer[i * 2 + 1] = attribute;
    }
}

void vga_text_console::write(char c, unsigned char fore, unsigned char back) {
    unsigned int offset = index(m_x, m_y) * 2;

    uint8_t attribute = (uint8_t)((back & 0xF) << 4) | (fore & 0xF);

    current_background = back;

    switch (c) {
        case '\n':
            m_x = 0;
            m_y++;
            scroll_up();
            break;
        case '\b':
            m_x--;
            if (m_x < 0) { m_x = 0; }
            offset         = index(m_x, m_y) * 2;
            m_buffer[offset] = ' ';
            break;
        default:
            m_buffer[offset]     = c;
            m_buffer[offset + 1] = attribute;
            m_x++;
            break;
    }

    if (m_x >= width()) {
        m_x = 0;
        m_y++;
        scroll_up();
    }
    set_cursor();
}

void vga_text_console::scroll_up() {
    const int sz     = width() * 2;
    if (m_y >= height()) {
        for (size_t i = 1; i < (size_t)height(); i++) { memcpy(&m_buffer[sz * (i - 1)], &m_buffer[sz * i], sz); }

        size_t start = width() * (height() - 1) * 2;

        for (int i = 0; i < width(); i++) {
            m_buffer[start + (i * 2) + 0] = ' ';
            m_buffer[start + (i * 2) + 1] = (char)(current_background << 4);
        }

        m_y--;
    }
}

void vga_text_console::set_cursor() {
    uint16_t pos = (uint16_t)(m_y * width() + m_x);
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

int  vga_text_console::width() { return 80; }
int  vga_text_console::height() { return 25; }
bool vga_text_console::supports_cursor_position() { return true; }
void vga_text_console::setX(int x) { this->m_x = x; }
void vga_text_console::setY(int y) { this->m_y = y; }
int  vga_text_console::getX() { return m_x; }
int  vga_text_console::getY() { return m_y; }
