#pragma once
#include <kernel/hal/keycodes.h>
#include <kernel/types.h>
#include <kernel/util/ring_buffer.h>
#include <kernel/vfs/delegate.h>
#include <kernel/vfs/node.h>

namespace kernel {
namespace hal {
class keyboard : public vfs::delegate {
   public:
    struct kb_event {
        unsigned int keycode;
        bool         pressed;
    };

    util::ring_buffer<kb_event> event_buffer;

    // VFS delegate implementation
    virtual size_t read(vfs::node* n, void* buffer, size_t cursor_pos, size_t num_bytes) {
        int   elements_copied = 0;
        char* cbuf            = (char*)buffer;
        while (elements_copied < num_bytes && event_buffer.size() > 0) {
            auto evnt = event_buffer.top();
            if (evnt.pressed) {
                cbuf[cursor_pos + elements_copied] = evnt.keycode;
                elements_copied++;
            }
            event_buffer.pop();
        }
        return elements_copied;
    }

    virtual size_t write(vfs::node* n, void* buffer, size_t cursor_pos, size_t num_bytes) { return -1; }

    virtual char const* delegate_name() { return "ps2/keyboard"; }

   protected:
    void keyboard_set(unsigned int keycode, bool pressed) { key_status_map[keycode] = pressed; }
    bool key_status_map[VK_MAX_NUM];
};

uint8_t vk_to_ascii(unsigned int keycode);

}  // namespace hal
}  // namespace kernel