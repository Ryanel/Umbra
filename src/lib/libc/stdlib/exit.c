#define _SYS_SYSCALL_IDS
#include <stdlib.h>
#include <sys/syscall.h>

void exit(int status) {
    _SYSCALL2(SYS_SYSCALL_EXIT, status);
    while (1) {}
}