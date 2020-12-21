/// Exists in case a pure virtual function vtable is corrupted, so something gets called.
/// TODO: Make panic()
extern "C" void __cxa_pure_virtual() {}