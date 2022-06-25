#include <stdio.h>

#define _SYS_SYSCALL_IDS
#include <sys/syscall.h>

int puts(const char* s) {
    int len = strlen(s); // TODO: Proper return value
    char newline_buff[2];
    newline_buff[0] = '\n';
    newline_buff[1] = '\0';
    _do_syscall(SYS_SYSCALL_WRITE, stdout->_fileno, s, len);
    _do_syscall(SYS_SYSCALL_WRITE, stdout->_fileno, &newline_buff, 1);
    return len;
}