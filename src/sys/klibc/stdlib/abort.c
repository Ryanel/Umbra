#include <stdio.h>
#include <stdlib.h>
 
#ifdef __is_libk
extern void panic(const char* fmt, ...);
#endif

__attribute__((__noreturn__))
void abort() {
#ifdef __is_libk
	panic("abort()");
#else
	// TODO: Abnormally terminate the process as if by SIGABRT.
	printf("abort()\n");
#endif
	while (1) { }
}