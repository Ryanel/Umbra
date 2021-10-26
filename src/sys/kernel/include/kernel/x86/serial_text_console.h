#pragma once

#include <kernel/text_console.h>
#include <stdint.h>

namespace kernel {
namespace device {

/// A console that represents a serial deivce
class serial_text_console : public text_console {
   public:
    void init();
    void clear();

    void write(char c);
    void write_color(char c, char color);  //

    int width();
    int height();

    bool supports_color();
    bool supports_cursor_position();
    void setX(int x);
    void setY(int y);
    int  getX();
    int  getY();

    const int com1 = 0x3F8;

    int is_transmit_empty();

   private:
    int x = 0;
    int y = 0;
};

}  // namespace device
}  // namespace kernel