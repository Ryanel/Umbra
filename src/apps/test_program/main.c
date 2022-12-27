#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#define VK_INVALID    0
#define VK_ESC        1
#define VK_F1         2
#define VK_F2         3
#define VK_F3         4
#define VK_F4         5
#define VK_F5         6
#define VK_F6         7
#define VK_F7         8
#define VK_F8         9
#define VK_F9         10
#define VK_F10        11
#define VK_F11        12
#define VK_F12        12
#define VK_PRINTSCR   13
#define VK_SCROLLLOCK 14
#define VK_PAUSEBREAK 15

#define VK_BACKQUOTE 16
#define VK_1         17
#define VK_2         19
#define VK_3         20
#define VK_4         21
#define VK_5         22
#define VK_6         23
#define VK_7         24
#define VK_8         25
#define VK_9         26
#define VK_DASH      27
#define VK_EQUALS      28
#define VK_BACKSPACE 29

#define VK_TAB          30
#define VK_Q            31
#define VK_W            32
#define VK_E            33
#define VK_R            34
#define VK_T            35
#define VK_Y            36
#define VK_U            37
#define VK_I            38
#define VK_O            39
#define VK_P            40
#define VK_LEFTBRACKET  41
#define VK_RIGHTBRACKET 42
#define VK_ENTER        43

#define VK_CAPS      44
#define VK_A         45
#define VK_S         46
#define VK_D         47
#define VK_F         48
#define VK_G         49
#define VK_H         50
#define VK_J         51
#define VK_K         52
#define VK_L         53
#define VK_SEMICOLON 54
#define VK_QUOTE     55
#define VK_BACKSLASH 56

#define VK_LSHIFT   57
#define VK_Z        58
#define VK_X        59
#define VK_C        60
#define VK_V        61
#define VK_B        62
#define VK_N        63
#define VK_M        64
#define VK_COMMA    65
#define VK_PERIOD   66
#define VK_SLASH    67
#define VK_RSHIFT   68

#define VK_LCTRL  69
#define VK_LSUPER 70
#define VK_LALT   71
#define VK_SPACE  72
#define VK_RALT   73
#define VK_RSUPER 74
#define VK_RCTRL  75

#define VK_INSERT 76
#define VK_HOME   77
#define VK_PGUP   78
#define VK_DEL    79
#define VK_END    80
#define VK_PGDOWN 81

uint8_t vk_to_ascii(unsigned int keycode) {
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

int main() {
    printf("Test program written in C for the Umbra Operating System, using the Umbra libc!\n");

    while (1) { 
        int c = getchar();
        if (c) {
            putchar(vk_to_ascii(c));
        }
    }
    return 0;
}
