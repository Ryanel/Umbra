#include <console.h>
#include <panic.h>
#include <stdio.h>

void panic(char* s) {
    printf("loader: panic: %s\n", s);

    while (1) {}
}