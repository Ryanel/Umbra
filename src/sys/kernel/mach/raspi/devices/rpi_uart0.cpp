#include <kernel/arm/rpi/rpi.h>
#include <kernel/arm/rpi/rpi_uart0.h>
#include <kernel/delay.h>
#include <kernel/mem_utils.h>

#define UART0_DR   0x00201000
#define UART0_FR   0x00201018
#define UART0_IBRD 0x00201024
#define UART0_FBRD 0x00201028
#define UART0_LCRH 0x0020102C
#define UART0_CR   0x00201030
#define UART0_IMSC 0x00201038
#define UART0_ICR  0x00201044

using namespace kernel::device;

rpi_uart_text_console::rpi_uart_text_console() { init(); }

void rpi_uart_text_console::init() {
    unsigned int           reg = 0;
    kernel::arm::rpi::rpi& rpi = kernel::arm::rpi::rpi::get();

    // Initialize UART0 by disabiling it
    rpi.mmio_write(UART0_CR, 0);

    // Set up the clock divisor to support constant values.
    rpi.mbox[0] = 9 * 4;
    rpi.mbox[1] = MBOX_REQUEST;         // A request
    rpi.mbox[2] = MBOX_TAG_SETCLKRATE;  // Set the clock rate
    rpi.mbox[3] = 12;                   //
    rpi.mbox[4] = 8;                    //
    rpi.mbox[5] = 2;                    // UART clock
    rpi.mbox[6] = 4000000;              // @ 4 mhz
    rpi.mbox[7] = 0;                    // Clear Turbo
    rpi.mbox[8] = MBOX_TAG_LAST;
    rpi.mbox_call(MBOX_CH_PROP);

    // Set up UART to output to the GPIO pins.
    reg = rpi.mmio_read(GPFSEL1);
    reg &= ~((7 << 12) | (7 << 15));  // Pin 14 and 15
    reg |= (4 << 12) | (4 << 15);     // Alt 0
    rpi.mmio_write(GPFSEL1, reg);     //
    rpi.mmio_write(GPPUD, 0);         // Disable pull up control

    delay_cycles(150);                                 // Delay 150 cycles
    rpi.mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));  // Configure pulldown on pins 14 and 15.
    delay_cycles(150);                                 // Delay 150 cycles

    rpi.mmio_write(GPPUDCLK0, 0);

    // Set UART0 to 115200 baud, 8 bits, 1 stop, no parity.
    rpi.mmio_write(UART0_ICR, 0x7FF);  // Clear interrupts
    rpi.mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) |
                                   (1 << 10));  // Mask all interrupts
    rpi.mmio_write(UART0_IBRD, 2);              // 115200 baud
    rpi.mmio_write(UART0_FBRD, 0xB);            //
    rpi.mmio_write(UART0_LCRH, 0b11 << 5);      // 8n1
    rpi.mmio_write(UART0_CR, 0x301);            // Enable TX, RX, and FIFO Queue.
}

void rpi_uart_text_console::write(char c) {
    kernel::arm::rpi::rpi& rpi = kernel::arm::rpi::rpi::get();

    if (c == '\n') { write('\r'); }

    kernel::memory_utils::spin_flag_unset(rpi.mmio_base() + UART0_FR, 0x20);
    rpi.mmio_write(UART0_DR, c);
}

// Simulate a 80 x 25 text console without color or cursor.
void rpi_uart_text_console::clear() {}
void rpi_uart_text_console::write_color(char c, char color) { write(c); }
int  rpi_uart_text_console::width() { return 80; }
int  rpi_uart_text_console::height() { return 25; }
bool rpi_uart_text_console::supports_color() { return false; }
bool rpi_uart_text_console::supports_cursor_position() { return false; }
void rpi_uart_text_console::setX(int x) {}
void rpi_uart_text_console::setY(int y) {}
int  rpi_uart_text_console::getX() { return 0; }
int  rpi_uart_text_console::getY() { return 0; }
