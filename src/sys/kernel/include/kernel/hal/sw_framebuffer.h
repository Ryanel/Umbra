#pragma once
#include <stdint.h>

class sw_framebuffer {
   public:
    uint8_t*     buffer;
    unsigned int pitch;
    unsigned int width;
    unsigned int height;
    unsigned int bpp;

    void putpixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b) {
        uint8_t* pixaddr = buffer + (y * pitch) + (x * 3); // TODO, fix
        pixaddr[0]       = r;
        pixaddr[1]       = g;
        pixaddr[2]       = b;
    }
};