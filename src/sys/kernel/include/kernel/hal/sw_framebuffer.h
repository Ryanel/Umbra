#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum class fb_format { rgb, bgr };

class sw_framebuffer {
   public:
    uint8_t*     buffer;
    unsigned int pitch;
    unsigned int width;
    unsigned int height;
    unsigned int bpp;
    fb_format    format;

    sw_framebuffer() {}

    sw_framebuffer(uint8_t* buffer, unsigned int width, unsigned int height, unsigned int bpp, unsigned int pitch,
                   fb_format format) {
        this->buffer = buffer;
        this->width  = width;
        this->height = height;
        this->bpp    = bpp;
        this->pitch  = pitch;
    }

    void putpixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b) {
        uint8_t* pixaddr = buffer + (y * pitch) + (x * 3);

        if (format == fb_format::bgr) {
            pixaddr[0] = r;
            pixaddr[1] = g;
            pixaddr[2] = b;
        } else {
            pixaddr[0] = b;
            pixaddr[1] = g;
            pixaddr[2] = r;
        }
    }

    void clear() { memset(buffer, 0, height * pitch); }
    void lineclear(int y) { memset(buffer + (y * pitch), 0, pitch); }

    void linemove(int src, int dest) {
        uint8_t* ybuff_src  = buffer + (src * pitch);
        uint8_t* ybuff_dest = buffer + (dest * pitch);
        memmove(ybuff_dest, ybuff_src, pitch);
    }
};
