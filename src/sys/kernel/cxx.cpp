#include <kernel/panic.h>
#include <stdint.h>

/// Exists in case a pure virtual function vtable is corrupted, so something gets called.
extern "C" void __cxa_pure_virtual() { panic("__cxa_pure_virtual called, vtable corruption!"); }

// Global constructors
typedef void (*func_ptr)(void);
extern "C" func_ptr __init_array_start[0], __init_array_end[0];

void init_global_constructors(void) {
    for (func_ptr* func = __init_array_start; func != __init_array_end; func++) { (*func)(); }
}

// Stack smashing
// Taken from https://wiki.osdev.org/Stack_Smashing_Protector
#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

extern "C" __attribute__((noreturn)) void __stack_chk_fail(void) {
    panic("Stack was smashed");
    while (true) {}
}