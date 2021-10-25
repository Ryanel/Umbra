#pragma once

#include <stdint.h>

typedef struct register_frame {
    unsigned int  gs, fs, es, ds;
	unsigned int  edi, esi, ebp, ebx, edx, ecx, eax;  
	unsigned int  int_no, err_code; 
	unsigned int  eip, cs, eflags, esp, ss;
} __attribute__((__packed__)) register_frame_t; 