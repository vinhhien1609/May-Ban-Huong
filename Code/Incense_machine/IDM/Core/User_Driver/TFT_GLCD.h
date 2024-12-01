
#ifndef TFT_GLCD_H
#define TFT_GLCD_H

#include "main.h"
#include "All_75_Pic_256.h"
#include <stdbool.h>

#define uchar uint8_t
#define uint uint16_t
#define bit bool
	
#define color_brown   0x79A0//0x40c0
#define color_black   0x0000
#define color_white   0xffff
#define color_red     0xf800
#define color_green   0x00C0//0x07e0
#define color_blue    0x001f
#define color_dark_blue    0x0011
#define color_gray		0x8C95
#define color_yellow  color_red|color_green
#define color_cyan    color_green|color_blue
#define color_purple  color_red|color_blue

void LCD_init(void);
void Display_ON(void);
//Cor de fundo com 65k colors
void LCD_background(uint16_t color);
void LCD_background_Transparent(uint16_t color);
void test_lcd(void);
void LCD_CmdWrite(uint8_t cmd);
void LCD_DataWrite(uint8_t Data);
void Active_Window(uint XL,uint XR ,uint YT ,uint YB);
uint8_t  LCD_DataRead(void);
void LCD_TFT_Init(void);
void TFT_DrawString(char* str, int16_t x, int16_t y, int16_t color);
void TFT_DrawString_X4(char* str, int16_t x, int16_t y, int16_t color);
void TFT_DrawString_option(char* str, int16_t x, int16_t y, int16_t color, bool trans, uint8_t zoom);
void Displaypicture(uint8_t picnum);
void test_TFT_DrawString(int16_t x, int16_t y, int16_t color);

void  Draw_Circle_FILL(uint X,uint Y,uint R, uint b_color);
void drawRoundRect(uint x,uint y,uint w, uint h, uint r, uint color, bool fill);
void Text_Foreground_Color1(uint b_color);
void Memory_Clear(void);

void Scroll_Window(uint XL,uint XR ,uint YT ,uint YB);
///////////////Window scroll offset Settings
void Scroll(uint X,uint Y);
void RA8875_drawpixel(int16_t x,int16_t y,uint16_t color);
void Select_Layers_One(void);
void Select_Layers_Two(void);
void Access_layer(uchar layer);

void COLOR_mono(uint16_t point_x, uint16_t point_y, uint16_t w, uint16_t h);

#endif /* GLCD_H */
