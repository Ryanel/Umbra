#include <kernel/panic.h>

/// Exists in case a pure virtual function vtable is corrupted, so something gets called.
extern "C" void __cxa_pure_virtual() { panic("__cxa_pure_virtual called, vtable corruption!"); }

// Global constructors
typedef void (*func_ptr)(void);
extern "C" func_ptr __init_array_start[0], __init_array_end[0];

void init_global_constructors(void) {
    for (func_ptr* func = __init_array_start; func != __init_array_end; func++) { (*func)(); }
}