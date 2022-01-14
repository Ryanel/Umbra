#ifndef SYS_SYSCALL
#define SYS_SYSCALL 1


#ifdef _SYS_SYSCALL_IDS
#define SYS_SYSCALL_EXIT 0
#endif

void _do_syscall(uint64_t id, uint64_t p1, uint64_t p2, uint64_t p3);

#define _SYSCALL(x) _do_syscall(x,0,0,0)
#define _SYSCALL2(x,y) _do_syscall(x,y,0,0)

#endif