#include <decl/decl_FILE.h>
#include <stdio.h>
#include <sys/syscall.h>

void _fini();

FILE* stderr;
FILE* stdout;
FILE* stdin;

void initialize_standard_library(int argc, char* argv[], int envc, char* envp[]) {
    stdout = fopen("/dev/console", "r+");
    stdin  = fopen("/dev/keyboard", "r");
    stderr = fopen("/dev/console", "r+");
}

void exit(int status) {
    _fini();
    _exit(status);
    while (1) {}
}