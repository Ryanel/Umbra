#include <stdio.h>
#include <sys/syscall.h>

unsigned int files_no = 0;

// Todo, dynamically allocate.
#define MAX_FILES 255
FILE files[MAX_FILES];

FILE* fopen(const char* path, const char* mode) {
    uint64_t local_fid = _open(path, 0);

    unsigned int fno = files_no;

    if (files_no >= MAX_FILES) { return NULL; }
    FILE f;
    f._fileno = local_fid;

    files[fno] = f;
    fno++;
    return &files[fno];
}