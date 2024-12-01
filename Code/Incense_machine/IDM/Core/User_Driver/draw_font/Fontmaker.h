#ifndef __FONT__MAKER__
#define __FONT__MAKER__
#include "main.h"
typedef struct
{
	const uint8_t *font;
	const uint16_t *font_map;
}Font;

typedef struct
{
	const uint8_t *font;
	const uint16_t *font_map;
	const uint32_t *utf8;
	      uint16_t  utf8_size;
}__attribute__ ((packed)) uFont; 

uint16_t FontMakerUTF8_GetAddr(unsigned char *utf8_char,unsigned char *char_offset);
unsigned char FontMakerRead_font(int16_t x, int16_t y,  uint16_t txt,const Font *my_font);

uint16_t UserMakerUTF8_GetAddr(unsigned char *utf8_char,unsigned char *char_offset,const uFont *my_font);
unsigned char UserMakerRead_font(int16_t x, int16_t y,  uint16_t txt,const uFont *my_font);

//-----------------------------Cac ban copy code ma phan mem tao ra o day-------------------------------------------------------------------------
extern const Font FontDemo1;
extern const uFont FontBoLi11;
extern const uFont TiengTrungQuoc;
extern const Font font_16;
extern const uFont ufont_16;
extern const Font Arial_32;
extern const uFont uArial_32;
//-------------------------------------------------------------------------------------------------------------------------------------------------
#endif
