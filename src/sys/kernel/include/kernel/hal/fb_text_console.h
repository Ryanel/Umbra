#pragma once

#include <stdint.h>
#include <kernel/text_console.h>
#include <kernel/hal/sw_framebuffer.h>

namespace kernel {
namespace device {

/// A console that represents a textmode console drawn grapically.
class fb_text_console : public text_console {
   public:
    void init();
    void clear();
    
    void write(char c);
    void write_color(char c, char color);

    int width();
    int height();

    bool supports_color();
    bool supports_cursor_position();
    void setX(int x);
    void setY(int y);
    int getX();
    int getY();

    sw_framebuffer framebuffer;
    int font_width  = 8;
    int font_height = 8;
    
    void draw_char(int xpos, int ypos, char c, unsigned char fore, unsigned char back);
    void wrap();
   private:
    int x = 0;
    int y = 0;
};

}
}