
#ifndef TFT_GLCD_H
#define TFT_GLCD_H

#include "main.h"
void LCD_init(void);
void Display_ON(void);
//Cor de fundo com 65k colors
void LCD_background(uint16_t color);
void test_lcd(void);


#endif /* GLCD_H */
