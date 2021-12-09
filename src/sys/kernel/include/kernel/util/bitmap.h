#pragma once
#include <stdint.h>

class bitmap {
   public:
    inline bool test(uintptr_t index) { return (array[arr_index(index)] & (0x1 << arr_offset(index))) > 0; }
    inline void clear(uintptr_t index) { array[arr_index(index)] &= ~(0x1 << arr_offset(index)); }
    inline void set(uintptr_t index) { array[arr_index(index)] |= (0x1 << arr_offset(index)); }

    inline bool mark_if_clear(uintptr_t index) {
        auto idx = arr_index(index);
        auto off = arr_offset(index);
        if ((array[idx] & (0x1 << off)) == 0) {
            array[idx] |= (0x1 << off);
            return true;
        }
        return false;
    }

   private:
    inline uintptr_t arr_index(uintptr_t index) { return index / (8 * (sizeof(uintptr_t))); }
    inline uintptr_t arr_offset(uintptr_t index) { return index % (8 * (sizeof(uintptr_t))); }

   public:
    uintptr_t* array;
    uintptr_t  size_bytes;
};