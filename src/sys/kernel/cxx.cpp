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

// DSO and AtExit

void* __dso_handle = 0;

struct atexit_func_entry_t {
    /*
     * Each member is at least 4 bytes large. Such that each entry is 12bytes.
     * 128 * 12 = 1.5KB exact.
     **/
    void (*destructor_func)(void*);
    void* obj_ptr;
    void* dso_handle;
};

#define ATEXIT_MAX_FUNCS 128
atexit_func_entry_t __atexit_funcs[ATEXIT_MAX_FUNCS];
unsigned int        __atexit_func_count = 0;

extern "C" int __cxa_atexit(void (*f)(void*), void* objptr, void* dso) {
    if (__atexit_func_count >= ATEXIT_MAX_FUNCS) { return -1; };
    __atexit_funcs[__atexit_func_count].destructor_func = f;
    __atexit_funcs[__atexit_func_count].obj_ptr         = objptr;
    __atexit_funcs[__atexit_func_count].dso_handle      = dso;
    __atexit_func_count++;
    return 0;
};