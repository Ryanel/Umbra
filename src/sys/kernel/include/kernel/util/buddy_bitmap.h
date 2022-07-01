#pragma once

#include <kernel/util/bitmap.h>

// TODO: Implement a fully working version of this with multiple bitmap backings
class buddy_bitmap {
   public:
    int    granularity = 0;
    size_t size() const { return (granularity + 1) * 0x1000; }
    bitmap backing;

    void init(uintptr_t max_address) {
        size_t needed_ram = (max_address / size() / 8) + 1;
        // backing.array      = new uintptr_t[needed_ram];
        backing.size_bytes = needed_ram;
    }

    inline bool used(uintptr_t address) { return backing.test((address & 0xFFFFF000) / size()); }
    inline void mark(uintptr_t address) { backing.set((address & 0xFFFFF000) / size()); }
    inline bool mark_if_clear(uintptr_t address) { return backing.mark_if_clear((address & 0xFFFFF000) / size()); }
    inline void free(uintptr_t address) { backing.clear((address & 0xFFFFF000) / size()); }
};
