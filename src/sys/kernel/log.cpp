#include <kernel/log.h>
#include <kernel/spinlock.h>
#include <stdarg.h>
#include <stdio.h>

// This is THE kernel logger. Only one should be created.
kernel::log             kernel_logger;
kernel::utils::spinlock kprintf_spinlock;

kernel::log& kernel::log::get() { return kernel_logger; }

void kernel::log::init(device::text_console* device) {
    console = device;
    kprintf_spinlock.release();

    if (device->supports_color() && device->supports_cursor_position()) {
        device->clear();
        for (int x = 0; x < device->width(); x++) {
            device->write_color((char)205, 0x8B);  // ═
        }

        device->setX((device->width() / 2) - 2);
        device->setY(0);

        device->write_color('E', 0x8B);
        device->write_color('x', 0x8B);
        device->write_color('i', 0x8B);
        device->write_color('o', 0x8B);
        device->write_color('s', 0x8B);

        device->setY(1);
        device->setX(0);
    }
}

void kernel::log::write(char c) {
    // Print the given string.
    if (shouldBuffer) {
        // If we have buffering enabled, write to the buffer.
        write_buffer(c);
    } else {
        // Do not buffer the output, emit calls directly to the driver
        console->write(c);
    }
}

void kernel::log::write(const char* s) {
    // Do not attempt to write anything if there is no console!
    if (console == nullptr) { return; }

    // Print the given string.
    for (; (*s) != '\0'; s++) {
        if (shouldBuffer) {
            // If we have buffering enabled, write to the buffer.
            write_buffer(*s);
        } else {
            // Do not buffer the output, emit calls directly to the driver
            console->write(*s);
        }
    }
}

void kernel::log::write_buffer(char c) {
    buffer[buffer_index++] = c;

    if (buffer_index >= (LOG_BUFFER_MAX - 1) || c == '\n') { flush(); }
}

void kernel::log::flush() {
    if (buffer_index == 0) { return; }

    // Hardening: Prevent potential overflow if buffer_index is messed with and brought outside it's intended range.
    if (buffer_index > (LOG_BUFFER_MAX - 1)) { buffer_index = (LOG_BUFFER_MAX - 1); }

    // Flush any buffers to the screen
    for (unsigned int i = 0; i < buffer_index; i++) {
        console->write(buffer[i]);
        buffer[i] = 0;
    }

    buffer_index = 0;
}

extern "C" void kernel_c_shim_print_char_to_log(char c) { kernel_logger.write(c); }
extern "C" void kernel_c_shim_print_string_to_log(char* s) { kernel_logger.write(s); }

int kprintf(const char* fmt, ...) {
    // TODO: Bug in spinlock acquire where it spins to acquire lock?
    kprintf_spinlock.acquire();

    va_list arg;
    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);

    kprintf_spinlock.release();
    return 0;
}