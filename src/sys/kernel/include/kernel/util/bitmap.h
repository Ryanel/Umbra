#pragma once
#include <stdint.h>

class bitmap {
   public:
    bool test(uintptr_t index) { return (array[arr_index(index)] & (0x1 << arr_offset(index))) > 0; }
    void clear(uintptr_t index) { array[arr_index(index)] &= ~(0x1 << arr_offset(index)); }
    void set(uintptr_t index) { array[arr_index(index)] |= (0x1 << arr_offset(index)); }

   private:
    inline uintptr_t arr_index(uintptr_t index) { return index / (8 * (sizeof(uintptr_t))); }
    inline uintptr_t arr_offset(uintptr_t index) { return index % (8 * (sizeof(uintptr_t))); }

   public:
    uintptr_t* array;
    uintptr_t  size_bytes;
};