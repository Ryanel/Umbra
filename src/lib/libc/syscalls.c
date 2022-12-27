#define _SYS_SYSCALL_IDS
#include <stddef.h>
#include <sys/syscall.h>

void      _exit(int status) {
    _do_syscall(SYS_SYSCALL_EXIT, 0, 0, 0); 
    while(1) {} // Hang after this point, if we do not exit!
}
long long _write(uint64_t hnd, const void* buf, size_t count) {
    return _do_syscall(SYS_SYSCALL_WRITE, hnd, (uint64_t)buf, count);
}

long long _read(uint64_t hnd, const void* buf, size_t count) {
    return _do_syscall(SYS_SYSCALL_READ, hnd, (uint64_t)buf, count);
}

long long _open(const char* path, uint64_t flags) {
    return _do_syscall(SYS_SYSCALL_OPEN, (uint64_t)path, flags, 0);
}