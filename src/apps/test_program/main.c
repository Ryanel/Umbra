#include <stdint.h>
#include <string.h>

#include <sys/syscall.h>

const char* msg = "Hello, from a loaded program!";

int main() {
    _do_syscall(1, 1, (uintptr_t)msg, strlen(msg));  // write
    return 0;
}
