#pragma once
#include <kernel/hal/keycodes.h>
#include <kernel/types.h>
#include <kernel/util/ring_buffer.h>

namespace kernel {
namespace hal {
class keyboard {
   public:
    struct kb_event {
        unsigned int keycode;
        bool         pressed;
    };

    util::ring_buffer<kb_event> event_buffer;

   protected:
    void                        keyboard_set(unsigned int keycode, bool pressed) { key_status_map[keycode] = pressed; }
    bool                        key_status_map[VK_MAX_NUM];
};

uint8_t vk_to_ascii(unsigned int keycode);

}  // namespace hal
}  // namespace kernel