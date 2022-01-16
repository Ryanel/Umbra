#define _SYS_SYSCALL_IDS
#include <stddef.h>
#include <sys/syscall.h>

void      _exit(int status) { _do_syscall(SYS_SYSCALL_EXIT, 0, 0, 0); }
long long _write(int fd, const void* buf, size_t count) {
    return _do_syscall(SYS_SYSCALL_WRITE, fd, (uint64_t)buf, count);
}

long long _open(const char* path, uint64_t flags) {
    return _do_syscall(SYS_SYSCALL_OPEN, path, flags, 0);
}