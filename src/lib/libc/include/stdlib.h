#ifndef _STDLIB_H
#define _STDLIB_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void  abort(void);

int   atoi(const char*);
char* itoa(unsigned long long n, unsigned int base);

void  exit(int status);
int   atexit(void (*)(void));
char* getenv(const char* name);

void* calloc(size_t num, size_t sz);
void* malloc(size_t sz);
void  free(void*);

// Abs
int abs (int n);
long int labs (long int n);
long long int llabs (long long int n);

#ifdef __cplusplus
}
#endif

#endif