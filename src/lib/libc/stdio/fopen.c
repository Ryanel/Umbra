#include <stdio.h>
#include <sys/syscall.h>

// Todo, dynamically allocate.
#define MAX_FILES 255
FILE files[MAX_FILES];
unsigned int files_no = 0;

FILE* fopen(const char* path, const char* mode) {
    if (files_no >= MAX_FILES) { return NULL; }

    int local_fid = _open(path, 0);
    unsigned int fno = files_no;

    files[fno]._fileno = local_fid;
    files_no++;
    return &files[fno];
}