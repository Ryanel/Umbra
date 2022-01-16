#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

static void vprintf_print_helper(char c) { fwrite(&c, 1, 1, stdout); }
static void vprintf_print_helper_string(char* s) { fwrite(s, strlen(s), 1, stdout); }
static void vprintf_print_helper_string_sz(char* s, size_t sz) { fwrite(s, sz, 1, stdout); }

static void vprintf_print_padding_helper(unsigned int length, char* arg, bool fmt_length_zeropad) {
    if (length != 0) {
        size_t charsToPrint = length - strlen(arg);
        char   charToPrint  = ' ';
        if (fmt_length_zeropad) { charToPrint = '0'; }
        for (; charsToPrint > 0; charsToPrint--) { vprintf_print_helper(charToPrint); }
    }
}

int vprintf(const char* fmt, va_list arg) {
    // Format variables
    const char*  fmt_scan;
    char         fmt_specifier;
    unsigned int fmt_length;
    bool         fmt_length_zeropad = false;
    bool         fmt_left_justify   = false;

    // Argument variables

    uint32_t arg_int;
    uint64_t arg_int64;
    char*    arg_str;
    char*    opt_nonformat_str   = fmt;
    size_t   opt_nonformat_sz    = 0;
    bool     opt_nonformat_first = true;

    // Scan along the format string
    for (fmt_scan = fmt; *fmt_scan != '\0'; fmt_scan++) {
        if (*fmt_scan != '%') {
            if (opt_nonformat_first) {
                opt_nonformat_str   = fmt_scan;
                opt_nonformat_sz    = 0;
                opt_nonformat_first = false;
            }
            opt_nonformat_sz++;
            continue;
        } else {
            vprintf_print_helper_string_sz(opt_nonformat_str, opt_nonformat_sz);
            opt_nonformat_sz    = 0;
            opt_nonformat_first = true;
        }

        fmt_length_zeropad = false;
        fmt_left_justify   = false;
        fmt_length         = 0;

        // Skip to the format specifier
        fmt_scan++;

        if (*fmt_scan == '-') {
            fmt_left_justify = true;
            fmt_scan++;
        }

        // Check if we have a length argument
        if (*fmt_scan >= '0' && *fmt_scan <= '9') {
            // We have a length!
            char buffer[8];  // We only handle up to 7 digits
            int  buff_index = 0;
            memset(&buffer, 0, 8);
            fmt_length_zeropad = (*fmt_scan == '0');

            while (*fmt_scan >= '0' && *fmt_scan <= '9' && buff_index < 7) {
                buffer[buff_index++] = *fmt_scan;
                fmt_scan++;
            }

            int ln     = atoi((const char*)buffer);
            fmt_length = ln >= 0 ? (unsigned int)ln : 0;
        } else {
            fmt_length = 0;
        }

        // Don't allow zero padding when left justifying
        if (fmt_left_justify) { fmt_length_zeropad = false; }

        // Write format specifier
        fmt_specifier = *fmt_scan;

        switch (fmt_specifier) {
            case '%': vprintf_print_helper('%'); break;

            case 's':
                arg_str = va_arg(arg, char*);
                if (fmt_left_justify) {
                    vprintf_print_helper_string(arg_str);
                    vprintf_print_padding_helper(fmt_length, arg_str, false);
                } else {
                    vprintf_print_padding_helper(fmt_length, arg_str, false);
                    vprintf_print_helper_string(arg_str);
                }
                break;

            case 'c':
                arg_int = va_arg(arg, unsigned int);
                vprintf_print_helper((char)arg_int);
                break;

            case 'i':  // Improper
            case 'd':
                arg_int = va_arg(arg, unsigned int);
                arg_str = itoa(arg_int, 10);
                if (fmt_left_justify) {
                    vprintf_print_helper_string(arg_str);
                    vprintf_print_padding_helper(fmt_length, arg_str, false);
                } else {
                    vprintf_print_padding_helper(fmt_length, arg_str, fmt_length_zeropad);
                    vprintf_print_helper_string(arg_str);
                }
                break;

            case 'u':
                arg_int = va_arg(arg, uint32_t);
                arg_str = itoa(arg_int, 10);
                if (fmt_left_justify) {
                    vprintf_print_helper_string(arg_str);
                    vprintf_print_padding_helper(fmt_length, arg_str, false);
                } else {
                    vprintf_print_padding_helper(fmt_length, arg_str, fmt_length_zeropad);
                    vprintf_print_helper_string(arg_str);
                }
                break;

            case 'l':  // TODO: Wildy incorect to standard
                arg_int64 = va_arg(arg, uint64_t);
                arg_str   = itoa(arg_int64, 10);
                if (fmt_left_justify) {
                    vprintf_print_helper_string(arg_str);
                    vprintf_print_padding_helper(fmt_length, arg_str, false);
                } else {
                    vprintf_print_padding_helper(fmt_length, arg_str, false);
                    vprintf_print_helper_string(arg_str);
                }
                break;

            case 'x':
                arg_int = va_arg(arg, uint32_t);
                arg_str = itoa(arg_int, 16);
                if (fmt_left_justify) {
                    vprintf_print_helper_string(arg_str);
                    vprintf_print_padding_helper(fmt_length, arg_str, fmt_length_zeropad);
                } else {
                    vprintf_print_padding_helper(fmt_length, arg_str, fmt_length_zeropad);
                    vprintf_print_helper_string(arg_str);
                }

                break;

            case 'p':
                arg_int64 = va_arg(arg, uint64_t);
                arg_str   = itoa(arg_int64, 16);
                if (fmt_left_justify) {
                    vprintf_print_helper_string(arg_str);
                    vprintf_print_padding_helper(fmt_length, arg_str, fmt_length_zeropad);
                } else {
                    vprintf_print_padding_helper(fmt_length, arg_str, fmt_length_zeropad);
                    vprintf_print_helper_string(arg_str);
                }
                break;

            default: vprintf_print_helper(*fmt_scan); break;
        }
    }

    if (opt_nonformat_sz > 0) { vprintf_print_helper_string_sz(opt_nonformat_str, opt_nonformat_sz); }
    return 0;
}
