#ifndef _STRING_H
#define _STRING_H 1

#include <stddef.h>
#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t strlen(const char* str);
int    strcmp(const char* str1, const char* str2);
char*  strncpy(char* dest, const char* src, size_t count);
void*  memset(void* ptr, int value, size_t n);
void*  memcpy(void* dest, const void* src, size_t n);
void*  memmove(void* dest, const void* src, size_t n);

#ifdef __cplusplus
}
#endif
#endif