#include <stdio.h>
#include <sys/syscall.h>

size_t fwrite(const void* buf, size_t size, size_t count, FILE* fp) {
    if (size == 0 || count == 0) { return 0; }
    return (size_t)_write(fp->_fileno, buf, size * count);
}
