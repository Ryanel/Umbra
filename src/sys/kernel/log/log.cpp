#include <kernel/log.h>
#include <kernel/panic.h>
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

void kernel::log::init(device::text_console* device) {
    if (console_device_index >= LOG_DEVICE_MAX) { panic("Attempting to initalise too many log devices!"); }
    console[console_device_index] = device;
    console_device_index++;

    colorBack         = 0;
    colorFore         = 15;
    log_priority      = 0;
    shouldBuffer      = false;
    flush_on_newlines = true;

    device->init();
    device->clear(colorBack);
}

void kernel::log::write(char c) {
    // If we have buffering enabled, write to the buffer.
    if (shouldBuffer) {
        write_buffer(c);
    } else {
        console_print(c);
    }
}

void kernel::log::write(const char* s) {
    // Do not attempt to write anything if there is no console!
    if (console == nullptr) { return; }

    // Print the given string.
    for (; (*s) != '\0'; s++) { write(*s); }
}

void kernel::log::write_buffer(char c) {
    buffer[buffer_index++] = c;

    if (buffer_index >= (LOG_BUFFER_MAX - 1) || (c == '\n' && flush_on_newlines)) { flush(); }
}

void kernel::log::console_print(char c) {
    for (size_t i = 0; i < console_device_index; i++) { console[i]->write(c, colorFore, colorBack); }
}

void kernel::log::flush() {
    if (buffer_index == 0) { return; }

    // Hardening: Prevent potential overflow if buffer_index is messed with and brought outside it's intended range.
    if (buffer_index > (LOG_BUFFER_MAX - 1)) { buffer_index = (LOG_BUFFER_MAX - 1); }

    // Flush any buffers to the screen
    for (unsigned int i = 0; i < buffer_index; i++) {
        console_print(buffer[i]);
        buffer[i] = 0;
    }

    buffer_index = 0;
}

unsigned char kernel::log::log_print_common(const char* category, unsigned char color) {
    unsigned char oldFore   = kernel_logger.colorFore;
    kernel_logger.colorFore = color;

    uint64_t boot_ms        = kernel::time::boot_time_ns() / (uint64_t)1000000;
    uint32_t boot_secs      = (uint32_t)(boot_ms / 1000);
    uint32_t boot_hundreths = (uint32_t)(boot_ms % 1000);

    printf("%4d.%03d | %10s : ", boot_secs, boot_hundreths, category);

    return oldFore;
}

void kernel::log::status_log(const char* msg, unsigned char color) { get().status_log_int(msg, color); }

void kernel::log::status_log_int(const char* msg, unsigned char color) {
    flush();

    unsigned char oldFore = kernel_logger.colorFore;
    int           dest_x  = strlen(msg) + 2;

    for (size_t i = 0; i < console_device_index; i++) { console[i]->setX(console[i]->width() - dest_x - 1); }

    kernel_logger.colorFore = 0xF;
    write("[");
    kernel_logger.colorFore = color;
    write(msg);
    kernel_logger.colorFore = 0xF;
    write("]");
    kernel_logger.colorFore = oldFore;
    write("\n");
    flush();
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
        kernel_logger.colorFore = oldFore;                                \
        kernel_log_lock.release();                                        \
    }

LOG_BODY(trace, 0x8, 1);
LOG_BODY(debug, 0x7, 2);
LOG_BODY(info, 0x9, 3);
LOG_BODY(warn, 0xE, 4);
LOG_BODY(error, 0xC, 5);
LOG_BODY(critical, 0xC, 6);

#undef LOG_BODY