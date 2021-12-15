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

    constexpr fb_color(unsigned char r, unsigned char g, unsigned char b) : m_r(r), m_g(g), m_b(b) {}
};

/// A console that represents a textmode console drawn grapically.
class fb_text_console : public text_console {
   public:
    fb_text_console() {}

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

   private:
    int           m_x = 0;
    int           m_y = 0;
    unsigned char m_last_bg;
};

}  // namespace device
}  // namespace kernel