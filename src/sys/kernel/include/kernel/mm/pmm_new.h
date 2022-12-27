#pragma once

#include <kernel/util/result.h>

namespace kernel {
namespace mm {

// The page allocator determines if a page frame is free or not.
// It uses 2 different techniques to keep track of free pages:
// 1. A stack of recently-freed addresses
// 2. A bitmap of free addresses.
class page_allocator {
   public:
    void init();
    void mark_free(uintptr_t frame);
    void mark_free(uintptr_t startFrame, uintptr_t endFrame);

    void mark_used(uintptr_t frame);
    void mark_used(uintptr_t startFrame, uintptr_t endFrame);

    bool is_free(uintptr_t frame);
    bool is_free(uintptr_t startFrame, uintptr_t endFrame);

    uintptr_t get_frame();
    result_t get_frames(uintptr_t count, uintptr_t* buffer);
};
}  // namespace mm
}  // namespace kernel