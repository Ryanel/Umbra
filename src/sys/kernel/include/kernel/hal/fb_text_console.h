#pragma once

#include <kernel/hal/sw_framebuffer.h>
#include <kernel/text_console.h>
#include <stdint.h>

namespace kernel {
namespace device {
struct fb_color {
    unsigned char m_r;
    unsigned char m_g;
    unsigned char m_b;

    fb_color(unsigned char r, unsigned char g, unsigned char b) : m_r(r), m_g(g), m_b(b) {}
};

/// A console that represents a textmode console drawn grapically.
class fb_text_console : public text_console {
   public:
    void init();
    void clear(unsigned char bg);

    void write(char c, unsigned char fore, unsigned char back);

    int width();
    int height();

    bool supports_cursor_position();
    void setX(int x);
    void setY(int y);
    int  getX();
    int  getY();

    sw_framebuffer framebuffer;
    unsigned int   font_width  = 8;
    unsigned int   font_height = 8;

    void draw_char(int xpos, int ypos, char c, unsigned char fore, unsigned char back);

    fb_color color_table[16] = {
        {0, 0, 0},  // 0
        {0x5F, 0x81, 0x9D},  // dblue
        {0x8C, 0x94, 0x40},  // dgreen
        {0x5E, 0x8D, 0x87},  // dcyan
        {0xA5, 0x42, 0x42},  // dred
        {0x85, 0x67, 0x8F},  // dpurple
        {0xDE, 0x93, 0x5F},  // dyellow
        {0x70, 0x78, 0x80},  // lgray
        {0x47, 0x4B, 0x51},  // dgray
        {0x81, 0xA2, 0xBE},  // lblue
        {0xb5, 0xBD, 0x68},  // lgreen
        {0x8a, 0xBE, 0xB7},  // lcyan
        {0xcc, 0x66, 0x66},  // lred
        {0xb2, 0x94, 0xBB},  // lpurple
        {0xf0, 0xC6, 0x74},  // lyellow
        {0xc5, 0xc8, 0xc6},  // white
    };

   private:
    int           m_x = 0;
    int           m_y = 0;
    unsigned char m_last_bg;
};

}  // namespace device
}  // namespace kernel