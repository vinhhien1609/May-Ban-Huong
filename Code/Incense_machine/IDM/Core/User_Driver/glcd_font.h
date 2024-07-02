#ifndef FONT16X12_H
#define FONT16X12_H

#include "stdint.h"

typedef struct
{
    uint8_t width;
    uint8_t height;
    uint16_t startCharacter;
    uint16_t endCharacter;
    uint8_t* content;
} font_t;

extern font_t font6x8;
extern font_t font7x9;
extern font_t font16x12;
extern font_t font32x48;
extern font_t font24x32;
extern const uint8_t g_vnfont_8x15[];

extern unsigned int code_offset_table[];
extern unsigned char code_mapping_table[];
extern unsigned char code_width_table[];
extern unsigned char char_table[];
extern unsigned char heigh;

uint16_t vnfontFindPosition(uint16_t c);
//extern const unsigned char vietnamese_vowel_table[];

#endif  /* FONT16X12_H */
