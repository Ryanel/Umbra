#include <kernel/hal/fb_text_console.h>
#include <string.h>
#include <kernel/log.h>
#include <kernel/data/font-unscii.h>

using namespace kernel::device;

void fb_text_console::init() {
    clear();
    klogf("console", "Framebuffer text console is: %dx%d\n", width(), height());
        
}

void fb_text_console::clear() {
    x = 0;
    y = 0;

    //framebuffer.clear();
}


void fb_text_console::wrap() {
    if (x >= width()) {
        x = 0;
        y += 1;
    }
}

void fb_text_console::write(char c) {
    if(c == '\n') {
        y++;
        x = 0;
        return;
    }

    if(c == ' ') {
        x++;
        wrap();
        return;
    }

    draw_char(x, y, c, 0xF, 0x0);
    x++;
    wrap();
}

void fb_text_console::write_color(char c, char color) {
    framebuffer.putpixel(x,y, 0xFF, 0xFF, 0xFF);
}

void fb_text_console::draw_char(int xpos, int ypos, char c, unsigned char fore, unsigned char back) {
    unsigned int glyph_index = c - 32;

    if (c < 32) {
        glyph_index = 0;
    }

    unsigned int glyph_x, glyph_y;
    unsigned int screen_x = xpos * font_width;
    unsigned int screen_y = ypos * font_height;

    for (glyph_y = 0; glyph_y < font_height; glyph_y++) {
        for (glyph_x = 0; glyph_x < font_width; glyph_x++) {
            unsigned char data = font_unscii8_bitmap[(uint8_t)glyph_index][glyph_y];

#ifndef FONT_RENDER_INVERSE
            bool glyph_hit = (data >> (font_width - glyph_x)) & 1;
#else
            bool glyph_hit = (data >> (glyph_x)) & 1;
#endif

            if (glyph_hit) {
                framebuffer.putpixel(screen_x + glyph_x, screen_y + glyph_y, 0xFF, 0xFF, 0xFF);
            } else {
                framebuffer.putpixel(screen_x + glyph_x, screen_y + glyph_y, 0x0, 0x0, 0x0);
            }
        }
    }
}

int  fb_text_console::width() { return framebuffer.width / font_width; }
int  fb_text_console::height() { return framebuffer.height / font_height; }
bool fb_text_console::supports_color() { return true; }
bool fb_text_console::supports_cursor_position() { return true; }
void fb_text_console::setX(int x) { this->x = x; }
void fb_text_console::setY(int y) { this->y = y; }
int  fb_text_console::getX() { return x; }
int  fb_text_console::getY() { return y; }
