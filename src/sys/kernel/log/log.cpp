#include <kernel/hal/terminal.h>
#include <kernel/log.h>
#include <kernel/panic.h>
#include <kernel/text_console.h>
#include <kernel/time.h>
#include <kernel/util/spinlock.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// This is THE kernel logger. Only one should be created.
kernel::log  kernel_logger;
kernel::log& kernel::log::get() { return kernel_logger; }

kernel::util::spinlock kernel_log_lock;

extern "C" void kernel_c_shim_print_char_to_log(char c) { kernel_logger.write(c); }
extern "C" void kernel_c_shim_print_string_to_log(char* s) { kernel_logger.write(s); }

char kernel_log_buffer[LOG_TERM_BUFFER];

kernel::hal::terminal log_term(0, 0, 0, 0);

void kernel::log::init(device::text_console* device) {
    if (console_device_index >= LOG_DEVICE_MAX) { panic("Attempting to initalise too many log devices!"); }
    console[console_device_index] = device;
    console_device_index++;

    device->init();
    device->clear(colorBack);
}

void kernel::log::setup() {
    colorBack    = 0;
    colorFore    = 15;
    log_priority = 0;

    log_term   = kernel::hal::terminal(80, 25, (char*)&kernel_log_buffer, LOG_TERM_BUFFER);
    m_terminal = &log_term;

    m_terminal->set_output(console[0]);
}

void kernel::log::write(char c) { m_terminal->write_char(c); }

void kernel::log::write(const char* s) {
    // Do not attempt to write anything if there is no console!
    if (console == nullptr) { return; }

    // Print the given string.
    for (; (*s) != '\0'; s++) { write(*s); }
}

unsigned char kernel::log::log_print_common(const char* category, unsigned char color) {
    unsigned char oldFore   = kernel_logger.colorFore;
    write_color(color);

    uint64_t boot_ms        = kernel::time::boot_time_ns() / (uint64_t)1000000;
    uint32_t boot_secs      = (uint32_t)(boot_ms / 1000);
    uint32_t boot_hundreths = (uint32_t)(boot_ms % 1000);

    printf("%4d.%03d | %s: ", boot_secs, boot_hundreths, category);

    return oldFore;
}

void kernel::log::write_color(char fore) {
    log& l = get();
    l.write(0x1B);
    l.write('[');
    l.write('3');
    l.write('0' + (fore % 8));

    if (fore > 0x7) {
        l.write(';');
        l.write('1');
    }
    l.write('m');
}

#define LOG_BODY(LNAME, LCOLOR, LPRIO)                                    \
    void kernel::log::LNAME(const char* category, const char* fmt, ...) { \
        kernel_log_lock.acquire();                                        \
        if (LPRIO <= kernel_logger.log_priority) { return; }              \
        unsigned char oldFore = log_print_common(category, LCOLOR);       \
        va_list       arg;                                                \
        va_start(arg, fmt);                                               \
        vprintf(fmt, arg);                                                \
        va_end(arg);                                                      \
        write_color(oldFore);                                \
        kernel_log_lock.release();                                        \
    }

LOG_BODY(trace, 0x8, 1);
LOG_BODY(debug, 0x7, 2);
LOG_BODY(info, 0x9, 3);
LOG_BODY(warn, 0xE, 4);
LOG_BODY(error, 0xC, 5);
LOG_BODY(critical, 0xC, 6);

#undef LOG_BODY