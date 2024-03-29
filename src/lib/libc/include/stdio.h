#ifndef _STDIO_H
#define _STDIO_H

#include <decl/decl_FILE.h>
#include <stdarg.h>
#include <stddef.h>

#define SEEK_SET 0

#ifdef __cplusplus
extern "C" {
#endif

extern FILE* stderr;
#define stderr stderr

extern FILE* stdout;
#define stdout stdout

extern FILE* stdin;
#define stdin stdin

int    fclose(FILE*);
int    fflush(FILE*);
FILE*  fopen(const char*, const char*);
int    fprintf(FILE*, const char*, ...);
size_t fread(void*, size_t, size_t, FILE*);
int    fseek(FILE*, long, int);
long   ftell(FILE*);
size_t fwrite(const void*, size_t, size_t, FILE*);
void   setbuf(FILE*, char*);
int    vfprintf(FILE*, const char*, va_list);

int printf(const char* fmt, ...);
int puts(const char* str);

#ifdef __cplusplus
}
#endif

#endif
