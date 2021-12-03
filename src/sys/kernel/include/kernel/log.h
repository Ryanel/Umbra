#pragma once

#include <kernel/text_console.h>
#define LOG_BUFFER_MAX 80
#define LOG_DEVICE_MAX 5
namespace kernel {

/// The logging driver. The kernel calls into this class to get log output on the screen, and this redirects to the
/// appropreate devices.
class log {
   public:
    bool        shouldBuffer      = true;            ///< Should the log buffer output before sending it?
    bool        flush_on_newlines = true;            ///< Should we flush the buffer on newlines?
    void        init(device::text_console* device);  ///< Initialize the log with device as the logging console device
    void        write(char c);                       ///< Write a character to the log. May flush.
    void        write(const char* s);                ///< Write a string to the log. May flush mid string.
    void        flush();                             ///< Flushes the buffer (if enabled) to the output console
    static log& get();                               ///< Gets the log

    static void trace(const char* category, const char* fmt, ...);
    static void debug(const char* category, const char* fmt, ...);
    static void info(const char* category, const char* fmt, ...);
    static void warn(const char* category, const char* fmt, ...);
    static void error(const char* category, const char* fmt, ...);
    static void critical(const char* category, const char* fmt, ...);

    unsigned char colorFore    = 0xF;
    unsigned char colorBack    = 0x00;
    unsigned int  log_priority = 0;

   private:
    device::text_console* console[LOG_DEVICE_MAX];  ///< The console backends used for logging.
    char                  buffer[LOG_BUFFER_MAX];   ///< The buffer that holds unsent text
    unsigned int          buffer_index;             ///< The current character of the buffer
    void                  write_buffer(char c);     ///< Writes to the buffer
    void                  console_print(char c);
    unsigned int          console_device_index = 0;

   private:
    static unsigned char log_print_common(const char* category, unsigned char color);
};

};  // namespace kernel

/// Log to the kernel console.
/// \warning Will lock for the entire message.
int kprintf(const char* fmt, ...);
