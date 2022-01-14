#ifndef SYS_SYSCALL
#define SYS_SYSCALL 1


#ifdef _SYS_SYSCALL_IDS
#define SYS_SYSCALL_EXIT 0
#define SYS_SYSCALL_WRITE 1
#endif

void _do_syscall(unsigned long long id, unsigned long long p1, unsigned long long p2, unsigned long long p3);

#define _SYSCALL(x) _do_syscall(x,0,0,0)
#define _SYSCALL2(x,y) _do_syscall(x,y,0,0)

#endif