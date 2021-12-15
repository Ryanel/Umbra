#pragma once

#include <kernel/text_console.h>
#include <stdint.h>

namespace kernel {
namespace device {

/// A console that represents the 80x25 textmode VGA console.
class vga_text_console : public text_console {
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

    uintptr_t buffer_address = 0xC00B8000;
   private:
    int           m_x                = 0;
    int           m_y                = 0;
    unsigned char current_background = 0;

    // Helpers
    unsigned int index(int x, int y) { return ((width() * m_y) + m_x); }

    void scroll_up();
    void set_cursor();
};

}  // namespace device
}  // namespace kernel