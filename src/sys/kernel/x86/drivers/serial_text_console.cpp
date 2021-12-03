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

void serial_text_console::clear(unsigned char bg) {}
void serial_text_console::write(char c, unsigned char fore, unsigned char back) {
    if (fore != last_fore) {
        write_char(0x1B);
        write_char('[');
        write_char('3');
        write_char('0' + (fore % 8));

        if (fore > 0x7) {
            write_char(';');
            write_char('1');
        }
        write_char('m');

        last_fore = fore;
    }

    if (c == '\n') { write_char('\r'); }

    write_char(c);
}

void serial_text_console::write_char(char c) {
    while (is_transmit_empty() == 0) {}

    outb(com1, c);
}

int serial_text_console::width() { return 80; }
int serial_text_console::height() { return 25; }

bool serial_text_console::supports_cursor_position() { return false; }
void serial_text_console::setX(int x) { this->m_x = x; }
void serial_text_console::setY(int y) { this->m_y = y; }
int  serial_text_console::getX() { return m_x; }
int  serial_text_console::getY() { return m_y; }

int serial_text_console::is_transmit_empty() { return inb(com1 + 5) & 0x20; }
