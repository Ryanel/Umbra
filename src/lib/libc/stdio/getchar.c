#include <stdio.h>
#include <sys/syscall.h>

int getchar() {
    char c;
    intptr_t result = _read(stdin->_fileno, (uint64_t)&c, 1);

    // Filter out invalid values
    if (result <= 0) {
        return 0;
    }

    return c;
}