#pragma once
#include <kernel/hal/keycodes.h>
#include <kernel/types.h>
#include <kernel/util/ring_buffer.h>
#include <kernel/vfs/node.h>

namespace kernel {
namespace hal {
class keyboard : public vfs::vfs_delegate {
   public:
    struct kb_event {
        unsigned int keycode;
        bool         pressed;
    };

    util::ring_buffer<kb_event> event_buffer;

    // VFS delegate implementation
    virtual int read(vfs::vfs_node* node, size_t offset, size_t size, uint8_t* buffer) {
        int elements_copied = 0;
        while (elements_copied < size && event_buffer.size() > 0) {
            auto evnt = event_buffer.top();
            if (evnt.pressed) {
                buffer[offset + elements_copied] = evnt.keycode;
                elements_copied++;
            }
            event_buffer.pop();
        }
        return elements_copied;
    }

    virtual int write(vfs::vfs_node* node, size_t offset, size_t size, uint8_t* buffer) { return -1; }

    virtual char const* delegate_name() { return "ps2/keyboard"; }

   protected:
    void keyboard_set(unsigned int keycode, bool pressed) { key_status_map[keycode] = pressed; }
    bool key_status_map[VK_MAX_NUM];
};

uint8_t vk_to_ascii(unsigned int keycode);

}  // namespace hal
}  // namespace kernel