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

   private:
    const uintptr_t buffer_address = 0xC00B8000;

    int  x         = 0;
    int  y         = 0;
    unsigned char current_background = 0;

    // Helpers
    unsigned int index(int x, int y) { return ((width() * y) + x); }

    void scroll_up();
    void set_cursor();
};

}  // namespace device
}  // namespace kernel