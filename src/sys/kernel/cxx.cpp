#include <kernel/panic.h>
#include <stdint.h>

/// Exists in case a pure virtual function vtable is corrupted, so something gets called.
extern "C" void __cxa_pure_virtual() { panic("__cxa_pure_virtual called, vtable corruption!"); }

// Global constructors
extern void (*start_ctors)(void) __attribute__((weak));
extern void (*end_ctors)(void) __attribute__((weak));

void init_global_constructors(void) {
    uintptr_t* iterator = reinterpret_cast<uintptr_t*>(&start_ctors);
    while (iterator < reinterpret_cast<uintptr_t*>(&end_ctors)) {
        void (*fp)(void) = reinterpret_cast<void (*)(void)>(*iterator);
        fp();
        iterator++;
    }
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