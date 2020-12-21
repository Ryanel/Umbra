#pragma once

#include <kernel/text_console.h>
#include <stdint.h>

namespace kernel {
namespace device {

/// A console that represents a 80x25 textmode console
class rpi_uart_text_console : public text_console {
   public:
    rpi_uart_text_console();
    void clear();
    void write(char c);
    int  width();
    int  height();

    bool supports_color();
    bool supports_cursor_position();
    void setX(int x);
    void setY(int y);
    int  getX();
    int  getY();
    void write_color(char c, char color);

   private:
    void init(); ///< Initializes the console.
};

}  // namespace device
}  // namespace kernel