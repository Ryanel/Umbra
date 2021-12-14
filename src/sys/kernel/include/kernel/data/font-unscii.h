#include <stdint.h>

// http://viznut.fi/unscii/ , created by Viznut. Unscii is licensed under Public Domain
static const uint8_t font_unscii8_bitmap[][8] = {

    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0020
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00},  // U+0021
    {0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0022
    {0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00},  // U+0023
    {0x18, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x18, 0x00},  // U+0024
    {0x00, 0xC6, 0xCC, 0x18, 0x30, 0x66, 0xC6, 0x00},  // U+0025
    {0x38, 0x6C, 0x38, 0x76, 0xDC, 0xCC, 0x76, 0x00},  // U+0026
    {0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0027
    {0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00},  // U+0028
    {0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00},  // U+0029
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},  // U+002A
    {0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00},  // U+002B
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30},  // U+002C
    {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00},  // U+002D
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00},  // U+002E
    {0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x00},  // U+002F
    {0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00},  // U+0030
    {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00},  // U+0031
    {0x3C, 0x66, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00},  // U+0032
    {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00},  // U+0033
    {0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0x0C, 0x0C, 0x00},  // U+0034
    {0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00},  // U+0035
    {0x1C, 0x30, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00},  // U+0036
    {0x7E, 0x06, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x00},  // U+0037
    {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00},  // U+0038
    {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00},  // U+0039
    {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00},  // U+003A
    {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30},  // U+003B
    {0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x00},  // U+003C
    {0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00},  // U+003D
    {0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60, 0x00},  // U+003E
    {0x3C, 0x66, 0x06, 0x0C, 0x18, 0x00, 0x18, 0x00},  // U+003F
    {0x7C, 0xC6, 0xDE, 0xDE, 0xDE, 0xC0, 0x7C, 0x00},  // U+0040
    {0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00},  // U+0041
    {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00},  // U+0042
    {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00},  // U+0043
    {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00},  // U+0044
    {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00},  // U+0045
    {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00},  // U+0046
    {0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3E, 0x00},  // U+0047
    {0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00},  // U+0048
    {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00},  // U+0049
    {0x06, 0x06, 0x06, 0x06, 0x06, 0x66, 0x3C, 0x00},  // U+004A
    {0xC6, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0xC6, 0x00},  // U+004B
    {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00},  // U+004C
    {0xC6, 0xEE, 0xFE, 0xD6, 0xC6, 0xC6, 0xC6, 0x00},  // U+004D
    {0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0x00},  // U+004E
    {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},  // U+004F
    {0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00},  // U+0050
    {0x3C, 0x66, 0x66, 0x66, 0x66, 0x6C, 0x36, 0x00},  // U+0051
    {0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x00},  // U+0052
    {0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00},  // U+0053
    {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00},  // U+0054
    {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},  // U+0055
    {0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00},  // U+0056
    {0xC6, 0xC6, 0xC6, 0xD6, 0xFE, 0xEE, 0xC6, 0x00},  // U+0057
    {0xC3, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0xC3, 0x00},  // U+0058
    {0xC3, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x00},  // U+0059
    {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00},  // U+005A
    {0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C, 0x00},  // U+005B
    {0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x00},  // U+005C
    {0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3C, 0x00},  // U+005D
    {0x10, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00},  // U+005E
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},  // U+005F
    {0x18, 0x0C, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+0060
    {0x00, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00},  // U+0061
    {0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x00},  // U+0062
    {0x00, 0x00, 0x3C, 0x60, 0x60, 0x60, 0x3C, 0x00},  // U+0063
    {0x06, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x3E, 0x00},  // U+0064
    {0x00, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00},  // U+0065
    {0x1C, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x30, 0x00},  // U+0066
    {0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x7C},  // U+0067
    {0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00},  // U+0068
    {0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x1E, 0x00},  // U+0069
    {0x0C, 0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x78},  // U+006A
    {0x60, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0x00},  // U+006B
    {0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00},  // U+006C
    {0x00, 0x00, 0xCC, 0xFE, 0xD6, 0xD6, 0xC6, 0x00},  // U+006D
    {0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x00},  // U+006E
    {0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00},  // U+006F
    {0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60},  // U+0070
    {0x00, 0x00, 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x06},  // U+0071
    {0x00, 0x00, 0x7C, 0x66, 0x60, 0x60, 0x60, 0x00},  // U+0072
    {0x00, 0x00, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x00},  // U+0073
    {0x30, 0x30, 0x7E, 0x30, 0x30, 0x30, 0x1E, 0x00},  // U+0074
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00},  // U+0075
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00},  // U+0076
    {0x00, 0x00, 0xC6, 0xC6, 0xD6, 0x7C, 0x6C, 0x00},  // U+0077
    {0x00, 0x00, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0x00},  // U+0078
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x3C},  // U+0079
    {0x00, 0x00, 0x7E, 0x0C, 0x18, 0x30, 0x7E, 0x00},  // U+007A
    {0x0E, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0E, 0x00},  // U+007B
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},  // U+007C
    {0x70, 0x18, 0x18, 0x0E, 0x18, 0x18, 0x70, 0x00},  // U+007D
    {0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+007E
    {0xC0, 0xA0, 0xAE, 0xA4, 0xC4, 0x04, 0x04, 0x00},  // U+007F
    {0xE0, 0xA0, 0xEE, 0xAA, 0xEA, 0x0A, 0x0E, 0x00},  // U+0080
    {0xE0, 0xA0, 0xE4, 0xAC, 0xE4, 0x04, 0x0E, 0x00},  // U+0081
    {0xE0, 0xA0, 0xEE, 0xA2, 0xEE, 0x08, 0x0E, 0x00},  // U+0082
    {0xE0, 0xA0, 0xEE, 0xA2, 0xEE, 0x02, 0x0E, 0x00},  // U+0083
    {0xE0, 0x40, 0x4E, 0x4A, 0xEA, 0x0A, 0x0A, 0x00},  // U+0084
    {0xE0, 0xA0, 0xA8, 0xA8, 0xA8, 0x08, 0x0E, 0x00},  // U+0085
    {0xE0, 0x80, 0xEE, 0x28, 0xEE, 0x02, 0x0E, 0x00},  // U+0086
    {0xE0, 0x80, 0xCE, 0x88, 0xEE, 0x02, 0x0E, 0x00},  // U+0087
    {0xA0, 0xA0, 0xEE, 0xA8, 0xAE, 0x02, 0x0E, 0x00},  // U+0088
    {0xA0, 0xA0, 0xE2, 0xA2, 0xA2, 0x0A, 0x0E, 0x00},  // U+0089
    {0xA0, 0xA0, 0xAE, 0xA8, 0x4E, 0x02, 0x0E, 0x00},  // U+008A
    {0xE0, 0xA0, 0xEC, 0x8A, 0x8A, 0x0A, 0x0C, 0x00},  // U+008B
    {0xE0, 0xA0, 0xEA, 0x8A, 0x8A, 0x0A, 0x0E, 0x00},  // U+008C
    {0xC0, 0xA0, 0xCE, 0xA4, 0xA4, 0x04, 0x0E, 0x00},  // U+008D
    {0xE0, 0x80, 0xEE, 0x22, 0xEE, 0x08, 0x0E, 0x00},  // U+008E
    {0xE0, 0x80, 0xEE, 0x22, 0xE6, 0x02, 0x0E, 0x00},  // U+008F
    {0xC0, 0xA0, 0xAE, 0xA8, 0xC8, 0x08, 0x0E, 0x00},  // U+0090
    {0xE0, 0xA0, 0xE4, 0x8C, 0x84, 0x04, 0x0E, 0x00},  // U+0091
    {0xE0, 0xA0, 0xEE, 0x82, 0x8E, 0x08, 0x0E, 0x00},  // U+0092
    {0xE0, 0x80, 0xEE, 0x28, 0xEC, 0x08, 0x0E, 0x00},  // U+0093
    {0xE0, 0x80, 0x8E, 0x88, 0xE8, 0x08, 0x0E, 0x00},  // U+0094
    {0xA0, 0xE0, 0xEA, 0xAA, 0xAE, 0x0E, 0x0A, 0x00},  // U+0095
    {0xE0, 0x80, 0xEE, 0x2A, 0xEE, 0x08, 0x08, 0x00},  // U+0096
    {0xE0, 0x80, 0xCE, 0x8A, 0xEE, 0x08, 0x08, 0x00},  // U+0097
    {0xE0, 0xA0, 0xEE, 0x2A, 0xEE, 0x0A, 0x0E, 0x00},  // U+0098
    {0xE0, 0xA0, 0xEE, 0x2A, 0xEE, 0x02, 0x0E, 0x00},  // U+0099
    {0xE0, 0xA0, 0xEE, 0x2A, 0xEE, 0x0A, 0x0A, 0x00},  // U+009A
    {0xE0, 0x80, 0x8E, 0x88, 0xEE, 0x02, 0x0E, 0x00},  // U+009B
    {0xE0, 0x80, 0xEE, 0x24, 0xE4, 0x04, 0x04, 0x00},  // U+009C
    {0xE0, 0xA0, 0xAE, 0xA8, 0xEE, 0x02, 0x0E, 0x00},  // U+009D
    {0xE0, 0xA0, 0xEA, 0x8E, 0x8E, 0x0A, 0x0A, 0x00},  // U+009E
    {0xE0, 0xA0, 0xEE, 0xAA, 0xAE, 0x08, 0x08, 0x00},  // U+009F
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+00A0
    {0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00},  // U+00A1
    {0x00, 0x18, 0x7E, 0xD8, 0xD8, 0x7E, 0x18, 0x00},  // U+00A2
    {0x38, 0x6C, 0x60, 0xF0, 0x60, 0x66, 0xFC, 0x00},  // U+00A3
    {0x66, 0x3C, 0x66, 0x3C, 0x66, 0x00, 0x00, 0x00},  // U+00A4
    {0xC3, 0x66, 0x3C, 0x18, 0x3C, 0x18, 0x18, 0x00},  // U+00A5
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},  // U+00A6
    {0x3C, 0x60, 0x3C, 0x66, 0x66, 0x3C, 0x06, 0x3C},  // U+00A7
    {0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+00A8
    {0x7E, 0x81, 0x9D, 0xB1, 0xB1, 0x9D, 0x81, 0x7E},  // U+00A9
    {0x3C, 0x6C, 0x6C, 0x3E, 0x00, 0x7E, 0x00, 0x00},  // U+00AA
    {0x00, 0x33, 0x66, 0xCC, 0x66, 0x33, 0x00, 0x00},  // U+00AB
    {0x00, 0x7E, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00},  // U+00AC
    {0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00},  // U+00AD
    {0x7E, 0x81, 0xB9, 0xA5, 0xB9, 0xA5, 0x81, 0x7E},  // U+00AE
    {0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+00AF
    {0x3C, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+00B0
    {0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x7E, 0x00},  // U+00B1
    {0x70, 0x18, 0x30, 0x60, 0x78, 0x00, 0x00, 0x00},  // U+00B2
    {0x78, 0x0C, 0x18, 0x0C, 0x78, 0x00, 0x00, 0x00},  // U+00B3
    {0x0C, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00},  // U+00B4
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x7C, 0x60, 0xC0},  // U+00B5
    {0x3E, 0x7A, 0x7A, 0x3A, 0x1A, 0x1A, 0x1A, 0x00},  // U+00B6
    {0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00},  // U+00B7
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x18, 0x00},  // U+00B8
    {0x30, 0x70, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00},  // U+00B9
    {0x38, 0x6C, 0x6C, 0x38, 0x00, 0x7C, 0x00, 0x00},  // U+00BA
    {0x00, 0xCC, 0x66, 0x33, 0x66, 0xCC, 0x00, 0x00},  // U+00BB
    {0x43, 0xC6, 0x4C, 0x5A, 0x36, 0x6A, 0xCF, 0x02},  // U+00BC
    {0x40, 0xC6, 0x4C, 0x5E, 0x33, 0x66, 0xCC, 0x0F},  // U+00BD
    {0xC0, 0x23, 0x66, 0x2D, 0xDB, 0x35, 0x67, 0x01},  // U+00BE
    {0x18, 0x00, 0x18, 0x30, 0x60, 0x66, 0x3C, 0x00},  // U+00BF
    {0x70, 0x00, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x00},  // U+00C0
    {0x0E, 0x00, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x00},  // U+00C1
    {0x18, 0x66, 0x00, 0x3C, 0x66, 0x7E, 0x66, 0x00},  // U+00C2
    {0x76, 0xDC, 0x00, 0x3C, 0x66, 0x7E, 0x66, 0x00},  // U+00C3
    {0x66, 0x00, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x00},  // U+00C4
    {0x18, 0x18, 0x00, 0x3C, 0x66, 0x7E, 0x66, 0x00},  // U+00C5
    {0x3F, 0x6C, 0xCC, 0xFE, 0xCC, 0xCC, 0xCF, 0x00},  // U+00C6
    {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x18},  // U+00C7
    {0x70, 0x00, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x00},  // U+00C8
    {0x0E, 0x00, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x00},  // U+00C9
    {0x18, 0x66, 0x00, 0xFE, 0xF0, 0xC0, 0xFE, 0x00},  // U+00CA
    {0x66, 0x00, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x00},  // U+00CB
    {0x70, 0x00, 0x7E, 0x18, 0x18, 0x18, 0x7E, 0x00},  // U+00CC
    {0x0E, 0x00, 0x7E, 0x18, 0x18, 0x18, 0x7E, 0x00},  // U+00CD
    {0x18, 0x66, 0x00, 0x7E, 0x18, 0x18, 0x7E, 0x00},  // U+00CE
    {0x66, 0x00, 0x7E, 0x18, 0x18, 0x18, 0x7E, 0x00},  // U+00CF
    {0x78, 0x6C, 0x66, 0xF6, 0x66, 0x6C, 0x78, 0x00},  // U+00D0
    {0x76, 0xDC, 0x00, 0xC6, 0xF6, 0xDE, 0xC6, 0x00},  // U+00D1
    {0x70, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0x7C, 0x00},  // U+00D2
    {0x0E, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0x7C, 0x00},  // U+00D3
    {0x18, 0x66, 0x00, 0x7C, 0xC6, 0xC6, 0x7C, 0x00},  // U+00D4
    {0x76, 0xDC, 0x00, 0x7C, 0xC6, 0xC6, 0x7C, 0x00},  // U+00D5
    {0x66, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0x7C, 0x00},  // U+00D6
    {0x00, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0x00, 0x00},  // U+00D7
    {0x3E, 0x66, 0x6E, 0x7E, 0x76, 0x66, 0x7C, 0x00},  // U+00D8
    {0x70, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00},  // U+00D9
    {0x0E, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00},  // U+00DA
    {0x18, 0x66, 0x00, 0xC6, 0xC6, 0xC6, 0x7C, 0x00},  // U+00DB
    {0x66, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00},  // U+00DC
    {0x0E, 0x00, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x00},  // U+00DD
    {0xC0, 0xC0, 0xFC, 0xC6, 0xFC, 0xC0, 0xC0, 0x00},  // U+00DE
    {0x3C, 0x66, 0x66, 0x6C, 0x66, 0x66, 0x6C, 0x00},  // U+00DF
    {0x70, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00},  // U+00E0
    {0x0E, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00},  // U+00E1
    {0x18, 0x66, 0x00, 0x3E, 0x66, 0xC6, 0x7E, 0x00},  // U+00E2
    {0x76, 0xDC, 0x00, 0x3E, 0x66, 0xC6, 0x7E, 0x00},  // U+00E3
    {0x66, 0x00, 0x3C, 0x06, 0x3E, 0x66, 0x3E, 0x00},  // U+00E4
    {0x18, 0x18, 0x00, 0x3E, 0x66, 0xC6, 0x7E, 0x00},  // U+00E5
    {0x00, 0x00, 0x7E, 0x1B, 0x7F, 0xD8, 0x77, 0x00},  // U+00E6
    {0x00, 0x00, 0x3C, 0x60, 0x60, 0x60, 0x3C, 0x18},  // U+00E7
    {0x70, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00},  // U+00E8
    {0x0E, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00},  // U+00E9
    {0x18, 0x66, 0x00, 0x3C, 0x7E, 0x60, 0x3C, 0x00},  // U+00EA
    {0x66, 0x00, 0x3C, 0x66, 0x7E, 0x60, 0x3C, 0x00},  // U+00EB
    {0x70, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00},  // U+00EC
    {0x0E, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00},  // U+00ED
    {0x18, 0x66, 0x00, 0x38, 0x18, 0x18, 0x3C, 0x00},  // U+00EE
    {0x66, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00},  // U+00EF
    {0x0C, 0x3E, 0x0C, 0x7C, 0xCC, 0xCC, 0x78, 0x00},  // U+00F0
    {0x76, 0xDC, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x00},  // U+00F1
    {0x70, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00},  // U+00F2
    {0x0E, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00},  // U+00F3
    {0x18, 0x66, 0x00, 0x3C, 0x66, 0x66, 0x3C, 0x00},  // U+00F4
    {0x76, 0xDC, 0x00, 0x3C, 0x66, 0x66, 0x3C, 0x00},  // U+00F5
    {0x66, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00},  // U+00F6
    {0x18, 0x18, 0x00, 0x7E, 0x00, 0x18, 0x18, 0x00},  // U+00F7
    {0x00, 0x02, 0x7C, 0xCE, 0xD6, 0xE6, 0x7C, 0x80},  // U+00F8
    {0x70, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00},  // U+00F9
    {0x0E, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00},  // U+00FA
    {0x18, 0x66, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x00},  // U+00FB
    {0x66, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00},  // U+00FC
    {0x0E, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x3C},  // U+00FD
    {0x60, 0x60, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60},  // U+00FE
    {0x66, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x3C},  // U+00FF
};