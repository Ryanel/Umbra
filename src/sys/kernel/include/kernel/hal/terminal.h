#pragma once

#include <kernel/text_console.h>
#include <kernel/util/ring_buffer.h>
#include <kernel/vfs/node.h>

namespace kernel {
namespace hal {

class terminal : public vfs::vfs_delegate {
   public:
    device::text_console*   m_text_out;
    vfs::vfs_node*          m_in;
    bool                    m_graphical = false;
    size_t                  m_width;
    size_t                  m_height;
    int                     m_cursor_x = 0;
    int                     m_cursor_y = 0;
    char                    m_color_fg;
    char                    m_color_bg;
    util::ring_buffer<char> m_buffer;

    bool mode_control_code = false;
    char control_code_buff[16];
    int  control_code_index = 0;

    terminal(size_t width, size_t height, size_t capacity = (80 * 25 * 2))
        : m_text_out(nullptr),
          m_in(nullptr),
          m_graphical(false),
          m_width(width),
          m_height(height),
          m_buffer(capacity),
          m_color_bg(0x0),
          m_color_fg(0xF) {}

    terminal(size_t width, size_t height, char* buffer = 0, size_t capacity = (80 * 25 * 2))
        : m_text_out(nullptr),
          m_in(nullptr),
          m_graphical(false),
          m_width(width),
          m_height(height),
          m_buffer(buffer, capacity),
          m_color_bg(0x0),
          m_color_fg(0xF) {}

    void set_output(kernel::device::text_console* out) {
        m_graphical = false;
        m_text_out  = out;
    }

    void set_active() { refresh(); }

    void refresh() {
        size_t curIndex = 0;
        if (m_text_out == nullptr) { return; }
        if (m_buffer.capacity() <= 0) { return; }

        size_t        oldx  = m_cursor_x;
        size_t        oldy  = m_cursor_x;
        unsigned char oldFg = m_color_fg;
        unsigned char oldBg = m_color_bg;

        m_text_out->clear(m_color_bg);
        m_text_out->setX(0);
        m_text_out->setY(0);

        while (curIndex < m_buffer.size()) {
            write_char(m_buffer[curIndex], false);
            curIndex++;
        }

        m_cursor_x = oldx;
        m_cursor_y = oldy;
        m_color_bg = oldBg;
        m_color_fg = oldFg;
    }

    virtual void write_char(char c, bool write = true) {
        switch (c) {
            case '\n':
                m_cursor_x = 0;
                m_cursor_y++;
                m_text_out->write(c, m_color_fg, m_color_bg);
                break;
            case 0x1B:
                mode_control_code  = true;
                control_code_index = 0;
                break;
            case 'm':
                if (mode_control_code) {
                    // Process CSI control code
                    mode_control_code = false;
                    if (control_code_buff[0] != '[') { break; }

                    if (control_code_buff[1] == '3') {
                        char fg = control_code_buff[2] - '0';
                        if (control_code_buff[3] == ';' && control_code_buff[4] == '1') { fg += 0x8; }
                        m_color_fg = fg;
                    }

                    memset(&control_code_buff, 0, 16);
                } else {
                    m_cursor_x++;
                    m_text_out->write(c, m_color_fg, m_color_bg);
                }
                break;
            default:
                if (mode_control_code && control_code_index < 16) {
                    control_code_buff[control_code_index] = c;
                    control_code_index++;
                } else {
                    m_text_out->write(c, m_color_fg, m_color_bg);
                    m_cursor_x++;
                }
                break;
        }

        if (m_cursor_y > m_height) { m_cursor_y = m_height; }
        if (buffered() && write) { m_buffer.push_back(c); }
    }

    // VFS delegate implementation
    virtual int read(vfs::vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }
    virtual int write(vfs::vfs_node* node, size_t offset, size_t size, uint8_t* buffer) {
        assert(buffer != nullptr);
        assert(size >= 0);
        assert(offset < size);

        for (size_t i = 0; i < size; i++) { write_char(buffer[i]); }
        return -1;
    }

    bool                buffered() const { return m_buffer.capacity() > 0; }
    virtual char const* delegate_name() { return "terminal"; }
};

}  // namespace hal
}  // namespace kernel