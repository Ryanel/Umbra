#ifndef _STDLIB_H
#define _STDLIB_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void  abort(void);
int   atexit(void (*)(void));
int   atoi(const char*);
void  exit(int status);
char* getenv(const char*);

void* calloc(size_t, size_t);
void* malloc(size_t);
void  free(void*);

#ifdef __cplusplus
}
#endif

#endif