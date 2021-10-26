#include <kernel/panic.h>
#include <kernel/x86/ports.h>
#include <kernel/x86/serial_text_console.h>
#include <string.h>

using namespace kernel::device;

void serial_text_console::init() {
    outb(com1 + 1, 0x00);  // Disable all interrupts
    outb(com1 + 3, 0x80);  // Enable DLAB (set baud rate divisor)
    outb(com1 + 0, 0x03);  // Set divisor to 3 (lo byte) 38400 baud
    outb(com1 + 1, 0x00);  //                  (hi byte)
    outb(com1 + 3, 0x03);  // 8 bits, no parity, one stop bit
    outb(com1 + 2, 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
    outb(com1 + 4, 0x0B);  // IRQs enabled, RTS/DSR set
    outb(com1 + 4, 0x1E);  // Set in loopback mode, test the serial chip
    outb(com1 + 0, 0xAE);  // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if (inb(com1 + 0) != 0xAE) { panic("Serial console faulty"); }
    outb(com1 + 4, 0x0F);
}

void serial_text_console::clear() {}
void serial_text_console::write(char c) {
    if (c == '\n') { write('\r'); }

    while (is_transmit_empty() == 0) {}

    outb(com1, c);
}
void serial_text_console::write_color(char c, char color) {
    // First, write the ANSI control code
    //write('\u0001');
    //write('[');
    //write('3');
    //write('1');
    //write('m');
    // Now, write the character
    write(c);
}
int  serial_text_console::width() { return 80; }
int  serial_text_console::height() { return 25; }
bool serial_text_console::supports_color() { return false; }
bool serial_text_console::supports_cursor_position() { return false; }
void serial_text_console::setX(int x) { this->x = x; }
void serial_text_console::setY(int y) { this->y = y; }
int  serial_text_console::getX() { return x; }
int  serial_text_console::getY() { return y; }

int serial_text_console::is_transmit_empty() { return inb(com1 + 5) & 0x20; }
