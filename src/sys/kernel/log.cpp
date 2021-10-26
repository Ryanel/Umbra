#include <kernel/log.h>
#include <kernel/spinlock.h>
#include <stdarg.h>
#include <stdio.h>
#include <kernel/panic.h>

// This is THE kernel logger. Only one should be created.
kernel::log             kernel_logger;
kernel::utils::spinlock kprintf_spinlock;

kernel::log& kernel::log::get() { return kernel_logger; }

void kernel::log::init(device::text_console* device) {
    if(console_device_index >= LOG_DEVICE_MAX) {
        panic("Attempting to initalise too many log devices!");
    }
    console[console_device_index] = device;
    console_device_index++;
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
    for (; (*s) != '\0'; s++) {
        write(*s);
    }
}

void kernel::log::write_buffer(char c) {
    buffer[buffer_index++] = c;

    if (buffer_index >= (LOG_BUFFER_MAX - 1) || c == '\n') { flush(); }
}

void kernel::log::console_print(char c) {
    for (size_t i = 0; i < console_device_index; i++)
    {
        console[i]->write(c);
    }
    
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

int klogf(const char* category, const char* fmt, ...) {
    // TODO: Bug in spinlock acquire where it spins to acquire lock?
    kprintf("%8s: ", category);
    kprintf_spinlock.acquire();

    va_list arg;
    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);

    kprintf_spinlock.release();
    return 0;
}
