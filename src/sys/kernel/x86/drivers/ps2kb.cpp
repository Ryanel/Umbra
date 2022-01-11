#include <kernel/hal/keyboard.h>
#include <kernel/x86/ports.h>
#include <kernel/x86/ps2keyboard.h>
#include <stdio.h>
#include <kernel/hal/terminal.h>

unsigned int scan_table[128] = {
    VK_INVALID,                                                        // Error
    VK_ESC, VK_1, VK_2, VK_3, VK_4, VK_5, VK_5, VK_6, VK_7, VK_8, VK_9, VK_DASH, VK_EQUALS, 0, // ESC 1234567890-=
    VK_TAB, VK_Q, VK_W, VK_E, VK_R, VK_T, VK_Y, VK_U, VK_I, VK_O, VK_P, VK_LEFTBRACKET, VK_RIGHTBRACKET, // TAB QWERTYUIOP[]
    VK_ENTER,                                                          // Enter
    VK_LCTRL,                                                          // LCtrl
    VK_A,  VK_S, VK_D, VK_F, VK_G, VK_H, VK_J, VK_K, VK_L, VK_SEMICOLON, VK_QUOTE, // ASDFGHJKL;'
    0,                                                                 // 
    VK_LSHIFT,                                                         // LSHIFT
    VK_BACKSLASH,                                                      // Backslash
    VK_Z, VK_X, VK_C, VK_V, VK_B, VK_N, VK_M, VK_COMMA, VK_PERIOD, VK_SLASH, // ZXCVBNM,./
    VK_RSHIFT,                                                         // RSHIFT
    VK_LALT,                                                           // LALT
    VK_SPACE,                                                          // SPACE
    VK_CAPS,                                                           // CAPSLOCK
    VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, // F1-10                                    // F1-F10
    0,                                                                 // 69 - Num lock
    0,                                                                 // Scroll Lock
    VK_HOME,                                                           // Home key
    0,                                                                 // Up Arrow
    0,                                                                 // Page Up
    VK_DASH,                                                           // -
    0,                                                                 // Left Arrow
    0,                                                                 //
    0,                                                                 // Right Arrow
    VK_EQUALS,                                                         // +
    0,                                                                 // 79 - End key
    0,                                                                 // Down Arrow
    0,                                                                 // Page Down
    0,                                                                 // Insert Key
    0,                                                                 // Delete Key
    0, 0, 0,                                                           //
    VK_F11,                                                                 // F11 Key
    VK_F12,                                                                 // F12 Key
    0,                                                                 // All other keys are undefined
};

namespace kernel {
namespace driver {

void ps2keyboard::init() { kernel::interrupts::handler_register(33, this); }

bool ps2keyboard::handle_interrupt(register_frame_t* regs) {
    // We recieved a scancode. Translate and add to the circular buffer.
    unsigned char scancode = inb(data_port);
    bool          is_break = scancode & 0x80;
    auto          charcode = scancode & 0x7F;

    // Support extended character codes, for keypad / right side buttons
    if (scancode == 0xE0 || scancode == 0xE1) {
        this->extended_e0 = true;
        return true;
    }

    keyboard_set(scan_table[charcode], !is_break);
    event_buffer.push_back(kb_event{scan_table[charcode], !is_break});

    // View Log
    if (scan_table[charcode] == VK_ESC) {
        kernel::log::get().m_terminal->set_output(kernel::log::get().console[1]);
        kernel::log::get().m_terminal->refresh();
    }

    this->extended_e0 = false;  // Reset state
    return true;
}

}  // namespace driver
}  // namespace kernel