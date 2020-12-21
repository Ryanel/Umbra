#pragma once

#include <stdint.h>
#include <kernel/text_console.h>

namespace kernel {
namespace device {

/// A console that represents the 80x25 textmode VGA console.
class vga_text_console : public text_console {
   public:
    void init();
    void clear();
    
    void write(char c);
    void write_color(char c, char color); // 

    int width();
    int height();

    bool supports_color();
    bool supports_cursor_position();
    void setX(int x);
    void setY(int y);
    int getX();
    int getY();

   private:
    const uintptr_t buffer_address = 0x000B8000;
    
    int x = 0;
    int y = 0;
    char attribute = 0x0F;

    // Helpers
    unsigned int index(int x, int y) {
        return ((width() * y) + x);
    }
};

}
}