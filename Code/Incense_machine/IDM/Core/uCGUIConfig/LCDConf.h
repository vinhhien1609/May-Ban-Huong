#ifndef LCDCONF_H
#define LCDCONF_H


#define LCD_XSIZE          (800)
#define LCD_YSIZE          (480)
#define LCD_CONTROLLER     (8875)
#define LCD_BITSPERPIXEL   (16)
#define LCD_FIXEDPALETTE   (565)
//#define LCD_SWAP_RB        (1)
//#define LCD_SWAP_XY        (1)
#define LCD_INIT_CONTROLLER()  0
#endif /* LCDCONF_H */

#define Bank1_LCD_D    ((uint32_t)0x60000000)    //disp Data ram
#define Bank1_LCD_C    ((uint32_t)0x60020000)	 //disp Reg ram
