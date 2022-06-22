#include <kernel/data/font-unscii.h>
#include <kernel/hal/fb_text_console.h>
#include <kernel/log.h>
#include <string.h>

using namespace kernel::device;

static const fb_color color_table[16] = {
    {0x18, 0x1A, 0x1E},  // 0
    {0x5F, 0x81, 0x9D},  // dblue
    {0x8C, 0x94, 0x40},  // dgreen
    {0x5E, 0x8D, 0x87},  // dcyan
    {0xA5, 0x42, 0x42},  // dred
    {0x85, 0x67, 0x8F},  // dpurple
    {0xDE, 0x93, 0x5F},  // dyellow
    {0x70, 0x78, 0x80},  // lgray
    {0x37, 0x3B, 0x41},  // dgray
    {0x81, 0xA2, 0xBE},  // lblue
    {0xb5, 0xBD, 0x68},  // lgreen
    {0x8a, 0xBE, 0xB7},  // lcyan
    {0xcc, 0x66, 0x66},  // lred
    {0xb2, 0x94, 0xBB},  // lpurple
    {0xf0, 0xC6, 0x74},  // lyellow
    {0xc5, 0xc8, 0xc6},  // white
};

void fb_text_console::init() {
    font_height = 8;
    font_width  = 8;
    kernel::log::debug("console", "Framebuffer text console is: %dx%d\n", width(), height());
    clear(0x0);
}

void fb_text_console::clear(unsigned char bg) {
    m_x = 0;
    m_y = 0;

    fb_color bgcolor = color_table[bg];
    for (unsigned int yi = 0; yi < framebuffer.m_height; yi++) {
        for (unsigned int xi = 0; xi < framebuffer.m_width; xi++) {
            framebuffer.putpixel(xi, yi, bgcolor.m_r, bgcolor.m_g, bgcolor.m_b);
        }
    }
}

void fb_text_console::write(char c, unsigned char fore, unsigned char back) {
    m_last_bg = back;
    switch (c) {
        case '\n':
            m_y++;
            m_x = 0;
            break;
        default:
            draw_char(m_x, m_y, c, fore, back);
            m_x++;
            break;
    }

    // Wrap console around
    if (m_x >= width()) {
        m_x = 0;
        m_y += 1;
    }

    // Scroll if needed
    if (m_y >= (height())) {
        framebuffer.linemove(font_height, 0, font_height * (height() - 1));

        for (size_t line = 0; line < font_height; line++) {
            unsigned int y_line  = (height() - 1) * font_height + line;
            fb_color     bgcolor = color_table[m_last_bg];
            for (unsigned int xi = 0; xi < framebuffer.m_width; xi++) {
                framebuffer.putpixel(xi, y_line, bgcolor.m_r, bgcolor.m_g, bgcolor.m_b);
            }
        }
        m_y -= 1;
    }
}

void fb_text_console::draw_char(int xpos, int ypos, char c, unsigned char fore, unsigned char back) {
    unsigned int glyph_index = c - 32;
    if (c <= 32) { return; }

    unsigned int screen_x = xpos * font_width;
    unsigned int screen_y = ypos * font_height;

    fb_color forecolor = color_table[fore];
    fb_color backcolor = color_table[back];

    for (unsigned int glyph_y = 0; glyph_y < font_height; glyph_y++) {
        for (unsigned int glyph_x = 0; glyph_x < font_width; glyph_x++) {
            unsigned char data = font_unscii8_bitmap[(uint8_t)glyph_index][glyph_y];

#ifndef FONT_RENDER_INVERSE
            bool glyph_hit = (data >> (font_width - glyph_x)) & 1;
#else
            bool glyph_hit = (data >> (glyph_x)) & 1;
#endif

            if (glyph_hit) {
                framebuffer.putpixel(screen_x + glyph_x, screen_y + glyph_y, forecolor.m_r, forecolor.m_g,
                                     forecolor.m_b);
            } else {
                framebuffer.putpixel(screen_x + glyph_x, screen_y + glyph_y, backcolor.m_r, backcolor.m_g,
                                     backcolor.m_b);
            }
        }
    }
}

int  fb_text_console::width() { return framebuffer.m_width / font_width; }
int  fb_text_console::height() { return framebuffer.m_height / font_height; }
bool fb_text_console::supports_cursor_position() { return true; }
void fb_text_console::setX(int x) { this->m_x = x; }
void fb_text_console::setY(int y) { this->m_y = y; }
int  fb_text_console::getX() { return m_x; }
int  fb_text_console::getY() { return m_y; }
