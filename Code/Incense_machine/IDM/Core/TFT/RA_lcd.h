
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H

#include<stdint.h>

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_lib.h"


////STM32 
//	#define LCD_BASE0        ((u32)0x60020000)//for ra8875
//	#define LCD_BASE1        ((u32)0x60000000)//for ra8875

//	#define LCD_CmdWrite(cmd)	  *(vu16*) (LCD_BASE0)= (cmd);
//	#define LCD_DataWrite(data)   *(vu16*) (LCD_BASE1)= (data);
//	#define	LCD_StatusRead()	 *(vu16*) (LCD_BASE0) //
//	#define	LCD_DataRead()   	 *(vu16*) (LCD_BASE1) //

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

//DEFINE line
#define  APP_LINE_0                0
#define  APP_LINE_1               24
#define  APP_LINE_2               48
#define  APP_LINE_3               72
#define  APP_LINE_4               96
#define  APP_LINE_5              120
#define  APP_LINE_6              144
#define  APP_LINE_7              168
#define  APP_LINE_8              192
#define  APP_LINE_9              216

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define Line0          0
#define Line1          24
#define Line2          48
#define Line3          72
#define Line4          96
#define Line5          120
#define Line6          144
#define Line7          168
#define Line8          192
#define Line9          216
#define Line10         240
#define Line11         264
#define Line12         288
#define Line13         312
#define Line14         336
#define Line15         360
#define Line16         384
#define Line17         408
#define Line18         432
#define Line19         456
#define Line20         480
#define Line21         504
#define Line22         528
#define Line23         552
#define Line24         576

#define Horizontal     0x00
#define Vertical       0x01

#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t

#define uc32 uint32_t
#define uc16 uint16_t
#define uc8 uint8_t

#define vu32 uint32_t
#define vu16 uint16_t
#define vu8 uint8_t

#define vuc32 uint32_t
#define vuc16 uint16_t
#define vuc8 uint8_t

#define s32 int32_t

void LCD_Delay(unsigned long nCount);
void RA8875_HW_Reset(void);

void lcd_rst(void);

/*----- Medium layer function -----*/
void LCD_WriteRAM_Prepare(void);

void LCD_SetTextColor(u16 Color);
void LCD_SetBackColor(u16 Color);

void LCD_Clear(u16 Color);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_SetReadCursor(u16 Xpos, u16 Ypos);

u16 LCD_GetPoint(u16 x,u16 y);		 
void LCD_SetPoint(u16 x,u16 y,u16 point);
void LCD_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor);

void LCD_DisplayString(u16 X,u16 Y, char *ptr, u16 charColor, u16 bkColor);
void LCD_DisplayStringLine(u16 Line, char *ptr, u16 charColor, u16 bkColor);
void LCD_SetDisplayWindow(u16 Xpos, u16 Ypos, u8 Height, u16 Width);

void LCD_DrawLine(u16 Xpos, u16 Ypos, u16 Length, u8 Direction);
void LCD_DrawRect(u16 Xpos, u16 Ypos, u16 Height, u16 Width);
void LCD_DrawCircle(u16 Xpos, u16 Ypos, u16 Radius);
void LCD_DrawMonoPict(uc32 *Pict);
void LCD_WriteBMP(u32 BmpAddress);


u16 GUI_Color565(u32 RGB);  // RGB颜色转为16位(565)

void GUI_Text(u16 x, u16 y, u8 *str, u16 len,u16 Color, u16 bkColor);
void GUI_Line(u16 x0, u16 y0, u16 x1, u16 y1,u16 color);  // 画线
void GUI_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill);  // 画园
void GUI_Rectangle(u16 x0, u16 y0, u16 x1, u16 y1,u16 color,u8 fill); // 画矩形
void GUI_Square(u16 x0, u16 y0, u16 with, u16 color,u8 fill);  // 画正方形


#endif /* __LCD_H */


