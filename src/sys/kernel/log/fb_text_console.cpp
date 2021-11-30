#include <kernel/data/font-unscii.h>
#include <kernel/hal/fb_text_console.h>
#include <kernel/log.h>
#include <kernel/scheduler.h>
#include <string.h>

using namespace kernel::device;

void fb_text_console::init() {
    kernel::log::debug("console", "Framebuffer text console is: %dx%d\n", width(), height());
    clear(0x0);
}

void fb_text_console::clear(unsigned char bg) {
    x = 0;
    y = 0;

    fb_color bgcolor = color_table[bg];

    for (unsigned int yi = 0; yi < framebuffer.height; yi++) {
        for (unsigned int xi = 0; xi < framebuffer.width; xi++) {
            framebuffer.putpixel(xi, yi, bgcolor.r, bgcolor.g, bgcolor.b);
        }
    }
}

void fb_text_console::wrap() {
    if (x >= width()) {
        x = 0;
        y += 1;
    }

    if (y >= (height())) {
        int y_i   = 1;
        int y_max = y;

        for (; y_i < y_max; y_i++) {
            for (size_t line = 0; line < font_height; line++) {
                unsigned int y_line = y_i * 8 + line;
                framebuffer.linemove(y_line, y_line - 8);
            }
        }

        for (size_t line = 0; line < font_height; line++) {
            unsigned int y_line  = (height() - 1) * 8 + line;
            fb_color     bgcolor = color_table[last_bg];
            for (unsigned int xi = 0; xi < framebuffer.width; xi++) {
                framebuffer.putpixel(xi, y_line, bgcolor.r, bgcolor.g, bgcolor.b);
            }
        }
        y -= 1;
    }
}

void fb_text_console::write(char c, unsigned char fore, unsigned char back) {
    if (c == '\n') {
        y++;
        x = 0;
        wrap();
        return;
    }

    last_bg = back;

    draw_char(x, y, c, fore, back);
    x++;
    wrap();
}

void fb_text_console::draw_char(int xpos, int ypos, char c, unsigned char fore, unsigned char back) {
    unsigned int glyph_index = c - 32;

    if (c < 32) { return; }

    unsigned int glyph_x, glyph_y;
    unsigned int screen_x = xpos * font_width;
    unsigned int screen_y = ypos * font_height;

    fb_color& forecolor = color_table[fore];
    fb_color& backcolor = color_table[back];

    for (glyph_y = 0; glyph_y < font_height; glyph_y++) {
        for (glyph_x = 0; glyph_x < font_width; glyph_x++) {
            unsigned char data = font_unscii8_bitmap[(uint8_t)glyph_index][glyph_y];

#ifndef FONT_RENDER_INVERSE
            bool glyph_hit = (data >> (font_width - glyph_x)) & 1;
#else
            bool glyph_hit = (data >> (glyph_x)) & 1;
#endif

            if (glyph_hit) {
                framebuffer.putpixel(screen_x + glyph_x, screen_y + glyph_y, forecolor.r, forecolor.g, forecolor.b);
            } else {
                framebuffer.putpixel(screen_x + glyph_x, screen_y + glyph_y, backcolor.r, backcolor.g, backcolor.b);
            }
        }
    }
}

int  fb_text_console::width() { return framebuffer.width / font_width; }
int  fb_text_console::height() { return framebuffer.height / font_height; }
bool fb_text_console::supports_cursor_position() { return true; }
void fb_text_console::setX(int x) { this->x = x; }
void fb_text_console::setY(int y) { this->y = y; }
int  fb_text_console::getX() { return x; }
int  fb_text_console::getY() { return y; }
