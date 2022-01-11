#include <kernel/hal/keyboard.h>

uint8_t kernel::hal::vk_to_ascii(unsigned int keycode) {
    switch (keycode) {
        case VK_BACKQUOTE: return '`';
        case VK_1: return '1';
        case VK_2: return '2';
        case VK_3: return '3';
        case VK_4: return '4';
        case VK_5: return '5';
        case VK_6: return '6';
        case VK_7: return '7';
        case VK_8: return '8';
        case VK_9: return '9';
        case VK_DASH: return '-';
        case VK_EQUALS: return '=';
        case VK_BACKSPACE: return '\b';
        case VK_TAB: return '\t';
        case VK_Q: return 'q';
        case VK_W: return 'w';
        case VK_E: return 'e';
        case VK_R: return 'r';
        case VK_T: return 't';
        case VK_Y: return 'y';
        case VK_U: return 'u';
        case VK_I: return 'i';
        case VK_O: return 'o';
        case VK_P: return 'p';
        case VK_A: return 'a';
        case VK_S: return 's';
        case VK_D: return 'd';
        case VK_F: return 'f';
        case VK_G: return 'g';
        case VK_H: return 'h';
        case VK_J: return 'j';
        case VK_K: return 'k';
        case VK_L: return 'l';
        case VK_SEMICOLON: return ';';
        case VK_QUOTE: return '\'';
        case VK_Z: return 'z';
        case VK_X: return 'x';
        case VK_C: return 'c';
        case VK_V: return 'v';
        case VK_B: return 'b';
        case VK_N: return 'n';
        case VK_M: return 'm';
        case VK_COMMA: return ',';
        case VK_PERIOD: return '.';
        case VK_SLASH: return '/';
        case VK_LEFTBRACKET: return '[';
        case VK_RIGHTBRACKET: return ']';
        case VK_ENTER: return '\n';
        case VK_SPACE: return ' ';
        default: return 0;
    }
}