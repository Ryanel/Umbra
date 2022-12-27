#include <stdio.h>
#include <string.h>

#define _SYS_SYSCALL_IDS
#include <sys/syscall.h>

int putchar(int c) {
    char buf[2];
    buf[0] = c;
    buf[1] = 0;  // Safety

    _do_syscall(SYS_SYSCALL_WRITE, stdout->_fileno, (uint64_t)&buf, 1);
    return c;
}
