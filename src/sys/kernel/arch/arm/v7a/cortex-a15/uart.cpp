#include <kernel/arm/uart_text_console.h>

using namespace kernel::device;

void uart_text_console::clear() {
    // Ignore clear requests for now.
}

void uart_text_console::write(char c) {
    if (wait_for_ldr) {
        while ((*(volatile uint32_t*)(base + 5) & 0x20) == 0) {
            // Wait
        }
    }
    *(volatile uint32_t*)(base) = c;
    if (c == '\n') { write('\r'); }
}

// Simulate a 80 x 25 text console
void uart_text_console::write_color(char c, char color) { write(c); }
int  uart_text_console::width() { return 80; }
int  uart_text_console::height() { return 25; }
void uart_text_console::set_address(uintptr_t address) { base = address; }
bool uart_text_console::supports_color() { return false; }
bool uart_text_console::supports_cursor_position() { return false; }
void uart_text_console::setX(int x) {}
void uart_text_console::setY(int y) {}
int  uart_text_console::getX() { return 0; }
int  uart_text_console::getY() { return 0; }
