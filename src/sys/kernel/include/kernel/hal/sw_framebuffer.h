#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum class fb_format { rgb, bgr };

class sw_framebuffer {
   public:
    uint8_t*     m_buffer;
    unsigned int m_pitch;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_bpp;
    fb_format    m_format;

    sw_framebuffer() {}

    sw_framebuffer(uint8_t* buffer, unsigned int width, unsigned int height, unsigned int bpp, unsigned int pitch,
                   fb_format format) {
        this->m_buffer = buffer;
        this->m_width  = width;
        this->m_height = height;
        this->m_bpp    = bpp;
        this->m_pitch  = pitch;
        this->m_format = format;
    }

    void putpixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b) {
        uint8_t* pixaddr = m_buffer + (y * m_pitch) + (x * 3);

        if (m_format == fb_format::rgb) {
            pixaddr[0] = r;
            pixaddr[1] = g;
            pixaddr[2] = b;
        } else {
            pixaddr[0] = b;
            pixaddr[1] = g;
            pixaddr[2] = r;
        }
    }

    void clear() { memset(m_buffer, 0, m_height * m_pitch); }
    void lineclear(int y) { memset(m_buffer + (y * m_pitch), 0, m_pitch); }

    void linemove(int src, int dest, int num_lines = 1) {
        uint8_t* ybuff_src  = m_buffer + (src * m_pitch);
        uint8_t* ybuff_dest = m_buffer + (dest * m_pitch);
        memmove(ybuff_dest, ybuff_src, m_pitch * num_lines);
    }
};
