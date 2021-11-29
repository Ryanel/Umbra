#include <kernel/panic.h>

/// Exists in case a pure virtual function vtable is corrupted, so something gets called.
extern "C" void __cxa_pure_virtual() { panic("__cxa_pure_virtual called, vtable corruption!"); }
