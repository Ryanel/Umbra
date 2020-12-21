#pragma once

#include <stdint.h>
#include <kernel/text_console.h>

namespace kernel {
namespace device {

/// A console that represents the 80x25 textmode VGA console.
class uart_text_console : public text_console {
   public:
    /// Wait for the data register?
    bool wait_for_ldr = true;
    void clear();
    void write(char c);
    int width();
    int height();

    /// Sets the address of the device
    void set_address(uintptr_t address);


    bool supports_color();
    bool supports_cursor_position();
    void setX(int x);
    void setY(int y);
    int getX();
    int getY();
    void write_color(char c, char color);

   private:
    /// The MMIO address of the device
    uintptr_t base = 0xB8000;
};

}
}