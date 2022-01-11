#pragma once

#define LOG_BUFFER_MAX  80
#define LOG_DEVICE_MAX  5
#define LOG_TERM_BUFFER 8192
// Forward declarations
namespace kernel {
namespace hal {
class terminal;
}
namespace device {
class text_console;
}
}  // namespace kernel

namespace kernel {

/// The logging driver. The kernel calls into this class to get log output on the screen, and this redirects to the
/// appropreate devices.
class log {
   public:
    void        init(device::text_console* device);  ///< Initialize the log with device as the logging console device
    void        setup();                             ///< Initialises the log proper.
    void        write(char c);                       ///< Write a character to the log. May flush.
    void        write(const char* s);                ///< Write a string to the log. May flush mid string.
    static log& get();                               ///< Gets the log

    static void trace(const char* category, const char* fmt, ...);
    static void debug(const char* category, const char* fmt, ...);
    static void info(const char* category, const char* fmt, ...);
    static void warn(const char* category, const char* fmt, ...);
    static void error(const char* category, const char* fmt, ...);
    static void critical(const char* category, const char* fmt, ...);

    static bool will_log(unsigned int prio) { return get().log_priority <= prio; }

    unsigned char colorFore    = 0xF;
    unsigned char colorBack    = 0x00;
    unsigned int  log_priority = 0;

    static void write_color(char fg);

    device::text_console* console[LOG_DEVICE_MAX];  ///< The console backends used for logging.
    hal::terminal*        m_terminal;

   private:
    char         buffer[LOG_BUFFER_MAX];  ///< The buffer that holds unsent text
    unsigned int buffer_index;            ///< The current character of the buffer
    void         write_buffer(char c);    ///< Writes to the buffer
    unsigned int console_device_index = 0;

   private:
    static unsigned char log_print_common(const char* category, unsigned char color);
};

};  // namespace kernel

/// Log to the kernel console.
/// \warning Will lock for the entire message.
int kprintf(const char* fmt, ...);
