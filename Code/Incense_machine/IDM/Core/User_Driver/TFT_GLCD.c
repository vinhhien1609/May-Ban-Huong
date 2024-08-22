
#include "TFT_glcd.h"
#include "glcd.h"
//#include "FreeRTOS.h"
//#include "task.h"
#include "stm32f1xx_hal.h"
#include "main.h"
//#include "back_ground.h"
#include "stm32f1xx_hal_spi.h"
#include "fatfs.h"
#include "ff.h"

#define uchar uint8_t
#define uint uint16_t
#define bit bool
	
#define color_brown   0x40c0
#define color_black   0x0000
#define color_white   0xffff
#define color_red     0xf800
#define color_green   0x07e0
#define color_blue    0x001f
#define color_yellow  color_red|color_green
#define color_cyan    color_green|color_blue
#define color_purple  color_red|color_blue


extern SPI_HandleTypeDef hspi2;

uint8_t isLCD_COLOR=0;

void LCD_CmdWrite(uint8_t cmd);
void LCD_DataWrite(uint8_t Data);
uint8_t  LCD_DataRead(void);
void LCD_TFT_Init(void);
void Chk_Busy(void);


void LCD_init(void)
{
	uint8_t temp=0;
	
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);	
	HAL_Delay(30);
	
	LCD_CmdWrite(0x88);      
	LCD_DataWrite(0x0C);
	HAL_Delay(10);
	LCD_CmdWrite(0x88);//FNCR1
	temp = LCD_DataRead();
	printf("Read_lcd: %d\r\n",temp);
	if(temp == 0x0C)
	{
		printf("LCD_COLOR_DETECTED\r\n");
		LCD_TFT_Init();
		isLCD_COLOR =1;
	}
	else
	{
		printf((const char*)"LCD_MONO_DETECTED\r\n");
		GLcd_Init();
		isLCD_COLOR =0;
	}
}


///////////////////////////////////////////////////////////////////////////
//#ifdef	LCD_MONO	//LCD color
unsigned char Pic1[1500] = { /* 0X00,0X01,0X78,0X00,0X64,0X00, */
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X60,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X01,0XFC,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X03,0XFF,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X03,0XFF,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,
0XEF,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XC1,
0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XC0,0X7C,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XC0,0X3E,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XE1,0XC0,0X1F,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XFF,0XE0,0X07,0X80,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XFF,0XE0,0X03,0XC0,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0E,0X7F,0XF0,0X01,0XE0,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X0E,0X03,0XF0,0X00,0XE0,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X0E,0X00,0X78,0X00,0X70,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X0E,0X00,0X1E,0X00,0X38,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X07,0X02,0X07,0X00,0X1C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X07,0X07,0XC1,0XC0,0X1E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X07,0X80,0XF8,0X60,0X0F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X3F,
0XE3,0X80,0X0E,0X38,0X07,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XFF,0XFF,
0XC0,0X03,0X9C,0X03,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XFF,0XFF,0XC0,
0X01,0XC6,0X03,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7F,0XFF,0XFF,0XE0,0X00,
0X73,0X01,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XFF,0XE0,0X1F,0XE0,0X00,0X39,
0X81,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFE,0X00,0X03,0XF0,0X03,0X9C,0XC0,
0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0XF8,0X01,0XFE,0X60,0X70,
0X00,0X00,0X00,0X00,0X00,0X00,0X1F,0XC0,0X00,0X00,0X3C,0X00,0X7F,0X30,0X78,0X00,
0X00,0X00,0X00,0X00,0X00,0X7F,0X00,0X00,0X00,0X0E,0X00,0X01,0X98,0X3C,0X00,0X00,
0X00,0X00,0X00,0X00,0XFE,0X00,0X00,0X00,0X07,0X00,0X00,0X8C,0X3C,0X00,0X00,0X00,
0X00,0X00,0X01,0XF8,0X00,0X00,0X00,0X01,0XC1,0XC0,0XC6,0X1E,0X00,0X00,0X00,0X00,
0X00,0X03,0XF0,0X00,0X00,0X00,0X00,0XF0,0XFF,0XC7,0X0E,0X00,0X00,0X00,0X00,0X00,
0X07,0XE0,0X00,0X00,0X00,0X00,0X3C,0X1F,0XE3,0XCF,0X80,0X00,0X00,0X00,0X00,0X0F,
0X80,0X00,0X00,0X00,0X00,0X0F,0X80,0X61,0XFF,0XC0,0X00,0X00,0X00,0X00,0X0F,0X00,
0X00,0X00,0X00,0X00,0X01,0XF0,0X40,0X3F,0XF0,0X00,0X00,0X00,0X00,0X1E,0X00,0X00,
0X00,0X00,0X00,0X00,0X3C,0X00,0X01,0XFC,0X00,0X00,0X00,0X00,0X3C,0X00,0X00,0X00,
0X00,0X00,0X00,0X07,0X80,0X00,0X7F,0X00,0X00,0X00,0X00,0X78,0X00,0X00,0X00,0X00,
0X00,0X00,0X01,0XF8,0X00,0X1F,0X80,0X00,0X00,0X00,0X70,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X3C,0X00,0X07,0XE0,0X00,0X00,0X00,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X01,0XF0,0X00,0X00,0X01,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X78,0X00,0X00,0X03,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X3C,0X00,0X00,0X03,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X40,0X1E,0X00,0X00,0X07,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X02,0XF0,
0X0E,0X00,0X00,0X07,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X02,0XF8,0X0F,
0X00,0X00,0X0E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XF8,0X07,0X00,
0X00,0X0E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0X80,0X00,
0X1E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0X80,0X00,0X1C,
0X00,0X00,0X00,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X80,0X00,0X1C,0X00,
0X00,0X00,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X80,0X00,0X1C,0X00,0X00,
0X00,0X60,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X80,0X00,0X38,0X00,0X00,0X00,
0X30,0X00,0X00,0X00,0X04,0X40,0X00,0X00,0X03,0X80,0X00,0X38,0X00,0X00,0X00,0X30,
0X00,0X00,0X00,0X0E,0X40,0X00,0X00,0X03,0X80,0X00,0X38,0X00,0X00,0X00,0X10,0X00,
0X00,0X00,0X07,0X60,0X00,0X00,0X03,0X80,0X00,0X70,0X00,0X00,0X00,0X18,0X00,0X00,
0X00,0X03,0X20,0X00,0X00,0X13,0X80,0X00,0X70,0X00,0X00,0X00,0X08,0X00,0X00,0X00,
0X01,0XB0,0X00,0X00,0X1F,0X00,0X00,0X70,0X00,0X00,0X00,0X08,0X00,0X00,0X00,0X01,
0X98,0X00,0X00,0X07,0X80,0X00,0XF0,0X00,0X00,0X00,0X0C,0X00,0X00,0X00,0X00,0X8C,
0X00,0X00,0X8F,0X80,0X00,0XE0,0X00,0X00,0X00,0X0C,0X00,0X00,0X00,0X00,0X86,0X00,
0X00,0XFF,0X00,0X01,0XE1,0X80,0X00,0X00,0X0C,0X00,0X00,0X00,0X00,0XC3,0X80,0X00,
0X7E,0X00,0X01,0XC1,0X80,0X00,0X00,0X0C,0X00,0X00,0X00,0X00,0XC1,0XF0,0XFC,0XFC,
0X00,0X03,0XC1,0X00,0X00,0X00,0X08,0X00,0X00,0X00,0X00,0XC0,0X7F,0XFF,0XF0,0X00,
0X03,0X83,0X00,0X00,0X00,0X18,0X00,0X00,0X00,0X00,0X40,0X07,0XFF,0XE0,0X00,0X07,
0X83,0X00,0X00,0X00,0X18,0X00,0X00,0X00,0X00,0X44,0X07,0XE0,0X00,0X00,0X0F,0X03,
0X00,0X00,0X00,0X30,0X00,0X00,0X00,0X00,0X43,0XFF,0XC0,0X00,0X00,0X0F,0X03,0X80,
0X00,0X00,0X30,0X00,0X00,0X00,0X00,0XC0,0XFF,0X00,0X00,0X00,0X1F,0X8F,0X98,0X00,
0X00,0X60,0X00,0X00,0X00,0X00,0XC0,0X1E,0X00,0X00,0X00,0X3F,0XFF,0X98,0X00,0X00,
0X60,0X00,0X00,0X00,0X00,0X80,0X3E,0X00,0X00,0X00,0X3F,0XFF,0XD8,0X00,0X00,0XC0,
0X00,0X00,0X00,0X00,0X80,0X7C,0X00,0X00,0X00,0X0F,0XFF,0XF0,0X00,0X01,0X80,0X00,
0X03,0X00,0X01,0X80,0XF8,0X00,0X00,0X00,0X00,0X01,0XE0,0X00,0X03,0X00,0X00,0X03,
0X00,0X01,0X07,0XF0,0X00,0X00,0X00,0X00,0X03,0XC0,0X00,0X07,0X00,0X00,0X03,0X00,
0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XC0,0X00,0X1F,0X00,0X00,0X03,0X00,0X07,
0XFF,0XF0,0X00,0X00,0X00,0X00,0X07,0X80,0X00,0X3F,0X80,0X00,0X07,0X80,0X0F,0X83,
0XF8,0X00,0X00,0X00,0X00,0X07,0X80,0X01,0XFF,0XF0,0X00,0X0F,0X80,0X0F,0XE0,0X7C,
0X00,0X00,0X00,0X00,0X03,0X80,0X0F,0XFF,0XFF,0X00,0XFF,0X80,0X00,0XFC,0X1C,0X00,
0X00,0X00,0X00,0X03,0XC0,0X03,0XFF,0XFF,0XFF,0XFF,0XC0,0X00,0X0F,0X0E,0X00,0X00,
0X00,0X00,0X03,0XE0,0X00,0X1F,0XFF,0XFF,0XFF,0XF8,0X00,0X03,0X8F,0X00,0X00,0X00,
0X00,0X01,0XFC,0X00,0X01,0XFF,0XFF,0XF0,0XFF,0XC0,0X00,0XC7,0X00,0X00,0X00,0X00,
0X00,0XFF,0XC0,0X00,0X3F,0X00,0X00,0X3F,0XF8,0X02,0X73,0X00,0X00,0X00,0X00,0X00,
0X3F,0XFC,0X00,0X0F,0X00,0X00,0X07,0XFE,0X00,0X33,0X00,0X00,0X00,0X00,0X00,0X07,
0XFF,0X80,0X27,0X80,0X00,0X00,0XFF,0X80,0X5F,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,
0XF0,0X0B,0X80,0X00,0X00,0X1F,0XE0,0X1F,0X00,0X00,0X00,0X00,0X00,0X00,0X1F,0XFE,
0X01,0X80,0X00,0X00,0X03,0XF8,0X0F,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XFF,0XC3,
0X80,0X00,0X00,0X00,0XFE,0X1E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X3F,0XF3,0X80,
0X00,0X00,0X00,0X3F,0XBE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0X80,0X00,
0X00,0X00,0X0F,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XFF,0X80,0X00,0X00,
0X00,0X03,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X3F,0X00,0X00,0X00,0X00,
0X01,0XFC,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1E,0X00,0X00,0X00,0X00,0X00,
0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X20,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,};
//*********4_wire_SPI****************************

void Chk_Busy(void);


uint32_t LCD_Index2Color_M332(int Index) {
  unsigned r, g, b;
  b = (Index & 3) * 85;
  g = ((Index >> 2) & 7) * 255 / 7;
  r = ((Index >> 5) & 7) * 255 / 7;
  return r + (g << 8) + (((uint32_t)b) << 16);
}
unsigned LCD_Color2Index_M332(uint32_t Color) {
  unsigned r, g, b;
  r = Color & 255;
  g = (Color >> 8 ) & 255;
  b = Color >> 16;
  r = (r * 7 + 127) / 255;
  g = (g * 7 + 127) / 255;
  b = (b + 42) / 85;
  return b + (g << 2) + (r << 5);
}

#define B_BITS 5
#define G_BITS 6
#define R_BITS 5

#define R_MASK ((1 << R_BITS) -1)
#define G_MASK ((1 << G_BITS) -1)
#define B_MASK ((1 << B_BITS) -1)

unsigned LCD_Color2Index_M565(uint32_t Color) {
  int r,g,b;
  r = (Color>> (8  - R_BITS)) & R_MASK;
  g = (Color>> (16 - G_BITS)) & G_MASK;
  b = (Color>> (24 - B_BITS)) & B_MASK;
  return b + (g << B_BITS) + (r << (G_BITS + B_BITS));
}

void LCD_CmdWrite(uint8_t cmd)
{ 
	//HAL_SPI_Init(&hspi2);
	 uint8_t  buff[2];
   buff[0] = 0x80;
   buff[1] = cmd;	
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
   //SCK = 1; 
   //SDA = 1; 
   //ZCS = 0;   
  // HAL_Delay(1);
	
	 HAL_SPI_Transmit(&hspi2, &buff[0], 2, 20);
//	 HAL_SPI_Transmit(&hspi2, &buff[1], 1, 0x1);
	 //HAL_SPI_Transmit(&hspi1, &cmd, 1, 0x1);
   //SPI_Write(0x80); 
   //SPI_Write(cmd);
	
  // HAL_Delay(1);
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
//	 HAL_SPI_DeInit(&hspi1);
	//ZCS = 1;
}
 
//----------------------//

void LCD_DataWrite(uint8_t Data)
{ 
	 //HAL_SPI_Init(&hspi2);
	 uint8_t  buff[2];
   buff[0] = 0x00;
   buff[1] = Data;
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	 HAL_SPI_Transmit(&hspi2, &buff[0], 2, 20);
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
}


////////////////Write command and parameter
void Write_Dir(uint8_t Cmd,uint8_t Data)
{
  LCD_CmdWrite(Cmd);
  LCD_DataWrite(Data);
	Chk_Busy();
}

void LCD_ColorWrite(uint16_t _color)
{
	 uint8_t data[2];
		data[0] = (_color >>8)&0xFF;
		data[1] = _color &0xFF;
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	 HAL_SPI_Transmit(&hspi2, data, 2, 20);
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);	
}
 
// void LCD_DataWriteM(uint8_t * pData, int NumWords)
//{ 
//	 uint8_t  buff[2], Data, Data1;
//	 Data =( *pData >> 8);
//	 Data1 = ((*(pData++)) & 0xFF);
//   buff[0] = 0x80;
//	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
////		HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
////	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
//	 HAL_SPI_Transmit(&hspi2, &buff[0], 1, 0x1);

// while(NumWords--)
// {
//	 HAL_SPI_Transmit(&hspi2, &Data, sizeof(Data), 0x1);
//   HAL_SPI_Transmit(&hspi2, &Data1, sizeof(Data1), 0x1);  
// }
//	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
//}

 void LCD_DataWriteM(uint8_t * pData, int NumWords)
{ 
	 uint8_t  buff[2], Data, Data1;
	 Data =( *pData >> 8);
	 Data1 = ((*(pData++)) & 0xFF);
   buff[0] = 0x80;
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	 HAL_SPI_Transmit(&hspi2, &buff[0], 1, 0x1);

 while(NumWords--)
 {
	 HAL_SPI_Transmit(&hspi2, &Data, sizeof(Data), 20);
   HAL_SPI_Transmit(&hspi2, &Data1, sizeof(Data1), 20);  
 }
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
}


//----------------------//
uint8_t  LCD_DataRead(void)
{
	
	//HAL_SPI_Init(&hspi1);
	 uint8_t  Data;
	 uint8_t buff[2]; 
	 buff[0] = 0x40;
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//   HAL_Delay(1);
	 HAL_SPI_Transmit(&hspi2, &buff[0], 1, 2);
   
	 HAL_SPI_Receive(&hspi2, &Data, sizeof(Data), 20);
 	
//   HAL_Delay(1);
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
	// HAL_SPI_DeInit(&hspi1);
	
  /*uchar Data;
   SCK = 1; 
   SDA = 1; 
   ZCS = 0;
   HAL_Delay(1);
   SPI_Write(0x40);  
   Data = SPI_Read();
   HAL_Delay(1);
   ZCS = 1;
 */
 return Data;
}  
//-----------------------//
uint8_t  LCD_StatusRead(void)
{
	
	//HAL_SPI_Init(&hspi1);
	 uint8_t  Data;
	 uint8_t buff[2]; 
	 buff[0] = 0xc0;
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
//   HAL_Delay(1);
	 HAL_SPI_Transmit(&hspi2, &buff[0], 1, 20);
   
	 HAL_SPI_Receive(&hspi2, &Data, sizeof(Data), 20);
 	
//   HAL_Delay(1);
	 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
	// HAL_SPI_DeInit(&hspi1);
	/*
  	uint8_t  Data;
 
   SCK = 1; 
   SDA = 1; 
   ZCS = 0;
   HAL_Delay(1);
   SPI_Write(0xc0); 
   Data = SPI_Read();
   HAL_Delay(1);
   ZCS = 1;
	 */
 return Data;
}

///////////////check busy
void Chk_Busy(void)
{
	uchar temp; 	
	do
	{
	temp=LCD_StatusRead();
	}while((temp&0x80)==0x80);		   
}
///////////////check bte busy
void Chk_BTE_Busy(void)
{
	uchar temp; 	
	do
	{
	temp=LCD_StatusRead();
	}while((temp&0x40)==0x40);		   
}
///////////////check dma busy
void Chk_DMA_Busy(void)
{
	uchar temp; 	
	do
	{
	LCD_CmdWrite(0xBF);	
	temp =LCD_DataRead();	
	}while((temp&0x01)==0x01);   
}
///////////////check dma busy
void Chk_FLASH_Busy(void)
{
	uchar temp; 	
	do
	{
	temp=LCD_StatusRead();		
	}while((temp&0x01)==0x01);   
}

/////////////////Set the working window area
void Active_Window(uint XL,uint XR ,uint YT ,uint YB)
{
	uchar temp;
    //setting active window X
	temp=XL;   
    LCD_CmdWrite(0x30);//HSAW0
	LCD_DataWrite(temp);
	temp=XL>>8;   
    LCD_CmdWrite(0x31);//HSAW1	   
	LCD_DataWrite(temp);

	temp=XR;   
    LCD_CmdWrite(0x34);//HEAW0
	LCD_DataWrite(temp);
	temp=XR>>8;   
    LCD_CmdWrite(0x35);//HEAW1	   
	LCD_DataWrite(temp);

    //setting active window Y
	temp=YT;   
    LCD_CmdWrite(0x32);//VSAW0
	LCD_DataWrite(temp);
	temp=YT>>8;   
    LCD_CmdWrite(0x33);//VSAW1	   
	LCD_DataWrite(temp);

	temp=YB;   
    LCD_CmdWrite(0x36);//VEAW0
	LCD_DataWrite(temp);
	temp=YB>>8;   
    LCD_CmdWrite(0x37);//VEAW1	   
	LCD_DataWrite(temp);
}

///////////////Background color settings
void Text_Background_Color1(uint b_color)
{
	
	LCD_CmdWrite(0x60);//BGCR0
	LCD_DataWrite((uchar)(b_color>>11));
	
	LCD_CmdWrite(0x61);//BGCR0
	LCD_DataWrite((uchar)(b_color>>5));
	
	LCD_CmdWrite(0x62);//BGCR0
	LCD_DataWrite((uchar)(b_color));
} 

///////////////Background color settings
void Text_Background_Color(uchar setR, uchar setG, uchar setB)
{
    LCD_CmdWrite(0x60);//BGCR0
	LCD_DataWrite(setR);
   
    LCD_CmdWrite(0x61);//BGCR0
	LCD_DataWrite(setG);

    LCD_CmdWrite(0x62);//BGCR0
	LCD_DataWrite(setB);
} 

////////////////Foreground color settings
void Text_Foreground_Color1(uint b_color)
{
	
	LCD_CmdWrite(0x63);//BGCR0
	LCD_DataWrite((uchar)(b_color>>11));
	
	LCD_CmdWrite(0x64);//BGCR0
	LCD_DataWrite((uchar)(b_color>>5));
	
	LCD_CmdWrite(0x65);//BGCR0
	LCD_DataWrite((uchar)(b_color));
} 

////////////////Foreground color settings
void Text_Foreground_Color(uchar setR,uchar setG,uchar setB)
{	    
    LCD_CmdWrite(0x63);//BGCR0
	LCD_DataWrite(setR);
   
    LCD_CmdWrite(0x64);//BGCR0
	LCD_DataWrite(setG);

    LCD_CmdWrite(0x65);//BGCR0��
	LCD_DataWrite(setB);
}
//////////////////BTE area size settings
void BTE_Size(width,height)
{
    uchar temp;
	temp=width;   
    LCD_CmdWrite(0x5c);//BET area width literacy
	LCD_DataWrite(temp);
	temp=width>>8;   
    LCD_CmdWrite(0x5d);//BET area width literacy	   
	LCD_DataWrite(temp);

	temp=height;   
    LCD_CmdWrite(0x5e);//BET area height literacy
	LCD_DataWrite(temp);
	temp=height>>8;   
    LCD_CmdWrite(0x5f);//BET area height literacy	   
	LCD_DataWrite(temp);
}		

////////////////////BTE starting position
void BTE_Source(uint SX,uint DX ,uint SY ,uint DY)
{
	uchar temp,temp1;
    
	temp=SX;   
    LCD_CmdWrite(0x54);//BTE horizontal position of read/write data
	LCD_DataWrite(temp);
	temp=SX>>8;   
    LCD_CmdWrite(0x55);//BTE horizontal position of read/write data   
	LCD_DataWrite(temp);

	temp=DX;
    LCD_CmdWrite(0x58);//BET written to the target horizontal position
	LCD_DataWrite(temp);
	temp=DX>>8;   
    LCD_CmdWrite(0x59);//BET written to the target horizontal position	   
	LCD_DataWrite(temp); 
    
	temp=SY;   
    LCD_CmdWrite(0x56);//BTE vertical position of read/write data
	LCD_DataWrite(temp);
	temp=SY>>8;   
    LCD_CmdWrite(0x57);
	temp1 = LCD_DataRead();
	temp1 &= 0x80;
    temp=temp|temp1; 
	LCD_CmdWrite(0x57);//BTE vertical position of read/write data  
	LCD_DataWrite(temp);

	temp=DY;   
    LCD_CmdWrite(0x5a);//BET written to the target  vertical  position
	LCD_DataWrite(temp);
	temp=DY>>8;   
    LCD_CmdWrite(0x5b);
	temp1 = LCD_DataRead();
	temp1 &= 0x80;
	temp=temp|temp1;	
	LCD_CmdWrite(0x5b);//BET written to the target  vertical  position 
	LCD_DataWrite(temp);
}				
///////////////Memory write position
void MemoryWrite_Position(uint X,uint Y)
{
	uchar temp;

	temp=X;   
    LCD_CmdWrite(0x46);
	LCD_DataWrite(temp);
	temp=X>>8;   
    LCD_CmdWrite(0x47);	   
	LCD_DataWrite(temp);

	temp=Y;   
    LCD_CmdWrite(0x48);
	LCD_DataWrite(temp);
	temp=Y>>8;   
    LCD_CmdWrite(0x49);	   
	LCD_DataWrite(temp);
}

////////////////Text write position
void FontWrite_Position(uint X,uint Y)
{
	uchar temp;
	temp=X;   
    LCD_CmdWrite(0x2A);
	LCD_DataWrite(temp);
	temp=X>>8;   
    LCD_CmdWrite(0x2B);	   
	LCD_DataWrite(temp);

	temp=Y;   
    LCD_CmdWrite(0x2C);
	LCD_DataWrite(temp);
	temp=Y>>8;   
    LCD_CmdWrite(0x2D);	   
	LCD_DataWrite(temp);
}

//////////////writing text
void String(uchar *str)
{   
    Write_Dir(0x40,0x80);//Set the character mode
	LCD_CmdWrite(0x02);
	while(*str != '\0')
	{
	 LCD_DataWrite(*str);
	 ++str;	 	
	 Chk_Busy();		
	} 
}

		

/////////////////Scroll window size
void Scroll_Window(uint XL,uint XR ,uint YT ,uint YB)
{
	uchar temp;    
	temp=XL;   
    LCD_CmdWrite(0x38);//HSSW0
	LCD_DataWrite(temp);
	temp=XL>>8;   
    LCD_CmdWrite(0x39);//HSSW1	   
	LCD_DataWrite(temp);

	temp=XR;   
    LCD_CmdWrite(0x3c);//HESW0
	LCD_DataWrite(temp);
	temp=XR>>8;   
    LCD_CmdWrite(0x3d);//HESW1	   
	LCD_DataWrite(temp);   
    
	temp=YT;   
    LCD_CmdWrite(0x3a);//VSSW0
	LCD_DataWrite(temp);
	temp=YT>>8;   
    LCD_CmdWrite(0x3b);//VSSW1	   
	LCD_DataWrite(temp);

	temp=YB;   
    LCD_CmdWrite(0x3e);//VESW0
	LCD_DataWrite(temp);
	temp=YB>>8;   
    LCD_CmdWrite(0x3f);//VESW1	   
	LCD_DataWrite(temp);
}  

///////////////Window scroll offset Settings
void Scroll(uint X,uint Y)
{
	uchar temp;
    
	temp=X;   
    LCD_CmdWrite(0x24);//HOFS0
	LCD_DataWrite(temp);
	temp=X>>8;   
    LCD_CmdWrite(0x25);//HOFS1	   
	LCD_DataWrite(temp);

	temp=Y;   
    LCD_CmdWrite(0x26);//VOFS0
	LCD_DataWrite(temp);
	temp=Y>>8;   
    LCD_CmdWrite(0x27);//VOFS1	   
	LCD_DataWrite(temp); 
}	   	  

///////////////The FLASH reading area   setting
void DMA_block_mode_size_setting(uint BWR,uint BHR,uint SPWR)
{
  	LCD_CmdWrite(0xB4);
  	LCD_DataWrite(BWR);
  	LCD_CmdWrite(0xB5);
  	LCD_DataWrite(BWR>>8);

  	LCD_CmdWrite(0xB6);
  	LCD_DataWrite(BHR);
  	LCD_CmdWrite(0xB7);
  	LCD_DataWrite(BHR>>8);

  	LCD_CmdWrite(0xB8);
  	LCD_DataWrite(SPWR);
  	LCD_CmdWrite(0xB9);
  	LCD_DataWrite(SPWR>>8);  
}

/////////////FLASH read start position Settings
void DMA_Start_address_setting(unsigned long set_address)
{ 
  	LCD_CmdWrite(0xB0);
  	LCD_DataWrite(set_address);

  	LCD_CmdWrite(0xB1);
  	LCD_DataWrite(set_address>>8);

		LCD_CmdWrite(0xB2);
  	LCD_DataWrite(set_address>>16);

//  	LCD_CmdWrite(0xB3);
//  	LCD_DataWrite(set_address>>24);
}
///////////drawing circle
void  Draw_Circle(uint X,uint Y,uint R)
{
	uchar temp;
    
	temp=X;   
    LCD_CmdWrite(0x99);
	LCD_DataWrite(temp);
	temp=X>>8;   
    LCD_CmdWrite(0x9a);	   
	LCD_DataWrite(temp);  
	  
	temp=Y;   
    LCD_CmdWrite(0x9b);
	LCD_DataWrite(temp);
	temp=Y>>8;   
    LCD_CmdWrite(0x9c);	   
	LCD_DataWrite(temp);

	temp=R;   
    LCD_CmdWrite(0x9d);
	LCD_DataWrite(temp);
} 

///////////drawing elliptic curve
void  Draw_Ellipse(uint X,uint Y,uint R1,uint R2)
{
	uchar temp;    
	temp=X;   
    LCD_CmdWrite(0xA5);
	LCD_DataWrite(temp);
	temp=X>>8;   
    LCD_CmdWrite(0xA6);	   
	LCD_DataWrite(temp);  
	  
	temp=Y;   
    LCD_CmdWrite(0xA7);
	LCD_DataWrite(temp);
	temp=Y>>8;   
    LCD_CmdWrite(0xA8);	   
	LCD_DataWrite(temp);

	temp=R1;   
    LCD_CmdWrite(0xA1);
	LCD_DataWrite(temp);
	temp=R1>>8;   
    LCD_CmdWrite(0xA2);	   
	LCD_DataWrite(temp);  
	  
	temp=R2;   
    LCD_CmdWrite(0xA3);
	LCD_DataWrite(temp);
	temp=R2>>8;   
    LCD_CmdWrite(0xA4);	   
	LCD_DataWrite(temp);
} 

///////////drawing line, rectangle, triangle
void Draw_Line(uint XS,uint XE ,uint YS,uint YE)
{	
    uchar temp;    
	temp=XS;   
    LCD_CmdWrite(0x91);
	LCD_DataWrite(temp);
	temp=XS>>8;   
    LCD_CmdWrite(0x92);	   
	LCD_DataWrite(temp);

	temp=XE;
    LCD_CmdWrite(0x95);
	LCD_DataWrite(temp);
	temp=XE>>8;   
    LCD_CmdWrite(0x96);	   
	LCD_DataWrite(temp);

	temp=YS;   
    LCD_CmdWrite(0x93);
	LCD_DataWrite(temp);
	temp=YS>>8;   
    LCD_CmdWrite(0x94);	   
	LCD_DataWrite(temp);

	temp=YE;   
    LCD_CmdWrite(0x97);
	LCD_DataWrite(temp);
	temp=YE>>8;   
    LCD_CmdWrite(0x98);	   
	LCD_DataWrite(temp);
}

////////////draw a triangle of three point 
void Draw_Triangle(uint X3,uint Y3)
{
    uchar temp;    
	temp=X3;   
    LCD_CmdWrite(0xA9);
	LCD_DataWrite(temp);
	temp=X3>>8;   
    LCD_CmdWrite(0xAA);	   
	LCD_DataWrite(temp);

	temp=Y3;
    LCD_CmdWrite(0xAB);
	LCD_DataWrite(temp);
	temp=Y3>>8;   
    LCD_CmdWrite(0xAC);	   
	LCD_DataWrite(temp);
}

/////////////Touch the interrupt judgment
bit Touch_Status(void)
{	
    uchar temp;
	LCD_CmdWrite(0xF1);//INTC	
	temp =LCD_DataRead();
	if ((temp&0x04)==0x04)
	/*temp=LCD_StatusRead();
	if ((temp&0x10)==0x10)*/
	return 1;
	else 
	return 0;
}
//////////check interrupt flag bit
bit Chk_INT(void)
{
	uchar temp; 	
	temp=LCD_StatusRead();
    if ((temp&0x20)==0x20)
	return 1;
	else 
	return 0;	   
}

bit Chk_INT2(void)
{
	uchar temp; 	
    LCD_CmdWrite(0x74);//INTC
	temp =LCD_DataRead();
    if ((temp&0x80)==0x80)
	return 1;
	else 
	return 0;	   
}

//-------------------------------------------//
//REG[2Eh]
void Font_size_16x16_8x16(void) 
{
 	uint8_t temp;
	LCD_CmdWrite(0x2E);//FNCR1
	temp = LCD_DataRead();
	temp &= 0x3f;
	LCD_DataWrite(temp);
}

//--------------------------------------------//
//REG[40h]
void Graphic_Mode(void)
{	uchar temp;
	LCD_CmdWrite(0x40);//MWCR0
	temp = LCD_DataRead();
	temp &= 0x7F ;
	LCD_DataWrite(temp);
}
//--------------------------------------------//
//REG[40h]
void Text_Mode(void)
{	uchar temp;
	LCD_CmdWrite(0x40);//MWCR0
	temp = LCD_DataRead();
	temp |= 0x80 ;
	LCD_DataWrite(temp);
}


/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    LCD_CmdWrite(0x46);
	LCD_DataWrite(Xpos);  
    LCD_CmdWrite(0x47);	   
	LCD_DataWrite(Xpos>>8);
    LCD_CmdWrite(0x48);
	LCD_DataWrite(Ypos);
    LCD_CmdWrite(0x49);	   
	LCD_DataWrite(Ypos>>8);
}

//---------------------------------------------//
//REG[8Eh]
void Stop_Memory_Clear(void)
{
	uchar temp;
	LCD_CmdWrite(0x8e);//MCLR
	temp = LCD_DataRead();
	temp &= 0x7F ;
	LCD_DataWrite(temp);
}

void Memory_Clear(void)
{
	uchar temp;
	LCD_CmdWrite(0x8e);//MCLR
	temp = LCD_DataRead();
	temp |= 0x7F ;
	LCD_DataWrite(0xFF); 		// clear active wwindow
	Chk_Busy(); 
	Stop_Memory_Clear();
}

void LCD_WriteRAM_Prepare(void)
{
  LCD_CmdWrite(0x02); //
}
////////////Show the picture of the flash
void Read_flash(uchar picnum)
{  
   uchar picnumtemp, temp;
	
   Write_Dir(0X06,0X00);//FLASH frequency setting
   Write_Dir(0X05,0X87);//FLASH setting flash 1, DMA_mode, 
	 Write_Dir(0XE0,0X01);//Direct Access Mode
	 Write_Dir(0XE1,0X00);//ADDR
	Write_Dir(0XE1,0X00);//ADDR
	Write_Dir(0XE1,0X00);//ADDR
	Chk_FLASH_Busy();
	printf("Dump flash:\r\n");
	for(int n=0; n<30; n++)
	{
		for(int n=0; n<16; n++)		
		{
			LCD_CmdWrite(0xE2);
			temp =LCD_DataRead();
			printf("%02x ",temp);
		}
		printf("\r\n");
	}
	Write_Dir(0XE0,0X00);//Direct Access Mode
	HAL_Delay(1000);
	Chk_FLASH_Busy();
}
////////////Show the picture of the flash
void Displaypicture(uchar picnum)
{  
   uchar picnumtemp;
//	Read_flash(1);
   Active_Window(0,799,0,479); 
   MemoryWrite_Position(0,0);//Memory write position	
//	Graphic_Mode();	
   Write_Dir(0X06,0X00);//FLASH frequency setting
   Write_Dir(0X05,0X87);//FLASH setting flash 1, DMA_mode, 
//	 Write_Dir(0XE0,0X00);//Direct Access Mode 

	Write_Dir(0XBF,0X02);//Stop DMA
	
	picnumtemp=picnum;
   DMA_Start_address_setting(384000*(picnumtemp-1));//DMA Start address setting
//   DMA_Start_address_setting(0);//DMA Start address setting	
   DMA_block_mode_size_setting(800,480,800);
	
	
//	LCD_WriteRAM_Prepare(); // Prepare to write GRAM 
//   Write_Dir(0XBF,0x02);//FLASH setting DMA Block
   Write_Dir(0XBF,0x03);//FLASH setting
	HAL_Delay(1000);
	Chk_DMA_Busy();
}



typedef struct BmpFileHeader {
   char bfType[2];	// 2 byte
   unsigned int bfSize;	// 4byte
   unsigned short int __bfReserved1;	// 2 byte
   unsigned short int __bfReserved2;	//2 byte
   unsigned long int bfOffBits;	// 4 byte
}  __attribute__((packed)) BMPFILEHEADER;

typedef struct BmpImageHeader {
   unsigned int biSize;	//4 byte
   unsigned int biWidth;		// 4byte
   unsigned int biHeight;	//4 byte
   unsigned short int biPlanes;	//2byte
   unsigned short int biBitCount;	//2byte bit per pixel
   unsigned int biCompression;		//4
   unsigned int biSizeImage;			//4
   int biXPelsPerMeter;		//4
   int biYPelPerMeter;		//4
   unsigned int biClrUsed;	//4
   unsigned int biClrImportant;	//4  
}  __attribute__((packed)) BMPIMAGEHEADER;

	BMPFILEHEADER header;
  BMPIMAGEHEADER imageHeader;

	FATFS fs;
  FRESULT res;
	FIL myfile;
	DIR dir;
	FILINFO finfo;

void drawBITMAP(void)
{


	uint8_t BUFFER[128];
	uint32_t wr=0;
		// mount the default drive
		res = f_mount(&fs, "", 0);
		if(res != FR_OK) {
				printf("f_mount() failed, res = %d\r\n", res);
		}
		else
			printf("SD_mouted\r\n");
	if(BSP_SD_Init() == MSD_OK)
	{
		printf("SD_init_OK\r\n");
		f_opendir(&dir,"");
		res = f_findfirst(&dir, &finfo, "", "*.bmp");
//		for(int t=0; t<13; t++)
//			if(finfo.fname[t]==',' && finfo.fname[t]==',' && finfo.fname[t]=='P'
		while(finfo.fname[0] && res == FR_OK)
		{
			printf("start Read file: %s %d\r\n", finfo.fname, res);
			res =f_open(&myfile, finfo.fname, FA_READ|FA_WRITE);
			if(res==FR_OK)
			{
				f_read(&myfile,&header, sizeof(BMPFILEHEADER), &wr);
				f_read(&myfile,&imageHeader, sizeof(BMPIMAGEHEADER), &wr);
				printf("sFile: %c%c\r\n", header.bfType[0], header.bfType[1]);
				printf("size bitmap: %dx%d ---%d\r\n", imageHeader.biWidth, imageHeader.biHeight, imageHeader.biBitCount);
//				if(header.bfType[0]=='B' && header.bfType[1]=='M')
				{
					f_lseek(&myfile, header.bfOffBits);// - sizeof(BMPFILEHEADER) - sizeof(BMPIMAGEHEADER) + 54);
					
					Graphic_Mode();
					Active_Window(0,imageHeader.biWidth-1,0,imageHeader.biHeight-1);
					LCD_SetCursor(0,0); 
					LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
					
					uint8_t buff_image[(imageHeader.biBitCount/8)*100];
					for(int n=0; n<imageHeader.biHeight; n++)
					{
						//printf("Reading file....\r\n");
		//				LCD_SetCursor(0,n);
//						for(int m=0; m<imageHeader.biWidth; m++)
//						{
//							uint16_t color=m;// = buff_image[0];

//							LCD_ColorWrite(color);
//						}
						uint16_t color;// = buff_image[0];
						uint8_t buff_dis[200];						
						for(int m=0; m<imageHeader.biWidth; m++)
						{
							f_read(&myfile, &buff_image, (imageHeader.biBitCount/8), &wr);

							for(int k=0; k<1; k++)
							{
								if(imageHeader.biBitCount==24)
									color = LCD_Color2Index_M332((uint32_t)(buff_image[k*3]<<16 | buff_image[k*3+1]<<8 | buff_image[k*3+2]));
								//color = LCD_Color2Index_M565((uint32_t)(buff_image[0]<<16 | buff_image[1]<<8 | buff_image[2]));
								buff_dis[k*2] =(color>>8)&0xFF;
								buff_dis[k*2+1] = color&0xFF ;
							}
//							LCD_ColorWrite(color);
							 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
							 HAL_SPI_Transmit(&hspi2, buff_dis, 2, 20);
							 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
						}
					}
				}
			}
			f_close(&myfile );
			HAL_Delay(2000);
			res = f_findnext(&dir, &finfo);
		}
		f_closedir(&dir);
	}
		else
			printf("SD_init_FAIL\r\n");	
}

void draw_BG(void)
{
  uint32_t index = 0, n;
//	Active_Window(200,439,100,243);
////    DMA_Start_address_setting(768000);
////    DMA_block_mode_size_setting(800,480,800);
////    DMA_Start_enable();	
//  LCD_SetCursor(0,0); 
//  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
//  for(index = 0; index < 144; index++)
//  {
//		for(n=0; n<240; n++)
//		{
//    LCD_DataWrite(ac8bit[index *240 + n]);
//		Chk_Busy();
//		}
//  }
	drawBITMAP();
}

void test_lcd()
{
//		Graphic_Mode();
    Active_Window(0,799,0,479);
		LCD_background(0xFF00);
		Write_Dir(0X8E,0X80);//Began to clear the screen (display window)
		HAL_Delay(100);
	
//		    Text_Foreground_Color1(color_white);//Set the foreground color
//			Text_Background_Color1(color_black);//Set the background color		
//			Active_Window(0,799,0,479);//Set the work window size
//			Write_Dir(0X8E,0X80);//Start screen clearing (display window)
//		  Chk_Busy();
//	
//			BTE_Size(15,68);
//    		Write_Dir(0x51,0xcc);//Raster Settings
//			for(int i=0;i<32;i++)
//			{
//				Text_Foreground_Color(i,0,0);
//				BTE_Source(0,i*15,0,0);//BTE starting position			 				  
//			    Write_Dir(0x50,0x80);//BET open
//			    Chk_BTE_Busy();
//			
//				Text_Foreground_Color(0,i*2,0);
//				BTE_Source(0,i*15,0,68);//BTE starting position		  
//			    Write_Dir(0x50,0x80);//BET open
//			    Chk_BTE_Busy();
//			
//				Text_Foreground_Color(0,0,i);
//				BTE_Source(0,i*15,0,136);//BTE starting position			 				  
//			    Write_Dir(0x50,0x80);//BET open
//			    Chk_BTE_Busy();
//			
//				Text_Foreground_Color(i,i*2,i);
//			    BTE_Source(0,i*15,0,204);//BTE starting position			 				  
//			    Write_Dir(0x50,0x80);//BET open
//			    Chk_BTE_Busy();
//			}	
//			HAL_Delay(1000);
				/////External characters of the functional test
//		    Text_Foreground_Color1(color_white);//Set the foreground color
//			Text_Background_Color1(color_black);//Set the background color	


			Active_Window(0,799,0,479);//Set the work window size
			Write_Dir(0X8E,0X80);//Start screen clearing (display window)
			Write_Dir(0XBF,0X02);//Stop DMA
		    Chk_Busy();			
			Write_Dir(0x05,0x28);// The waveform 3   2 byte dummy Cycle) 			
			Write_Dir(0x40,0x80);//text mode	
			Write_Dir(0x21,0x20);//Select the external character
			Write_Dir(0x06,0x03);//Set the frequency
			Write_Dir(0x2E,0x80);//Font Write Type Setting Register Set up 24 x24 character mode     spacing   0 
			Write_Dir(0x2F,0x81);//Serial Font ROM Setting GT23L32S4W normal
			Write_Dir(0x22,0x80);//Full alignment is enable.The text background color
			FontWrite_Position(40,350);//Text written to the position
		    Write_Dir(0x40,0x80);//Set the character mode
		    LCD_CmdWrite(0x02);//start write data
		    String("ShenZhen EastRising Technology .;LTD");	
		HAL_Delay(1000);				
//			////////////RA8875 internal input character test
//		    Text_Foreground_Color(0, 255, 0);//Set the foreground color
//		    Write_Dir(0x2E,0x01);//Set the characters mode 16 x16 / spacing 1
//				Write_Dir(0x2F,0x00);//Serial Font ROM Setting GT23L32S4W
//		    Write_Dir(0x40,0x80);//Set the character mode
//		    Write_Dir(0x21,0x10);//Select the internal CGROM  ISO/IEC 8859-1.
//		    FontWrite_Position(70,300);//Text written to the position
//		    Write_Dir(0x22,0x00);//Full alignment is disable.The text background color . Text don't rotation. Text don't zoom
//		    String("ShenZhen EastRising Technology .;LTD");	
//		HAL_Delay(1000);
		


//			//////////BTE Color Expansion
//		    Text_Background_Color1(color_purple);//Set the background color 
//		    Text_Foreground_Color1(color_yellow);//Set the foreground color
//		    BTE_Source(0,0,0,0);//BTE starting position
//		    BTE_Size(120,100);//BTE size setting
//		    Write_Dir(0x51,0x78);//Raster setting
//		    Write_Dir(0x50,0x80);//BET open
//			Chk_Busy();
//		    LCD_CmdWrite(0x02);//start write data
//  			for(int i=0;i<1500;i++)
//   			{
//			    LCD_DataWrite(Pic1[i]);
////				 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
////				 HAL_SPI_Transmit(&hspi2, &Pic1[i*100], 100, 20);
////				 HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 					
//				Chk_Busy();
//    		}
//   			Chk_BTE_Busy();		
//				HAL_Delay(1000);
		///////////Drawing curves
			Draw_Ellipse(120,68,100,50);
		    Text_Foreground_Color1(color_cyan);//Color Settings
		    Write_Dir(0XA0,0X10);//Setting parameters
		    Write_Dir(0XA0,0X90);//Start drawing
			HAL_Delay(1);
		    Write_Dir(0XA0,0X91);//Start drawing
			HAL_Delay(1);
		    Write_Dir(0XA0,0X92);//Start drawing
			HAL_Delay(1);
		    Write_Dir(0XA0,0X93);//Start drawing			
			
			HAL_Delay(1000);			
			Displaypicture(3);
			Write_Dir(0XBF,0X02);//Stop DMA
		    Chk_Busy();			
			Text_Foreground_Color1(color_red);//Color Settings
			Write_Dir(0x05,0x28);// The waveform 3   2 byte dummy Cycle) 			
			Write_Dir(0x40,0x80);//text mode	
			Write_Dir(0x21,0x20);//Select the external character
			Write_Dir(0x06,0x03);//Set the frequency
			Write_Dir(0x2E,0x80);//Font Write Type Setting Register Set up 24 x24 character mode     spacing   0 
			Write_Dir(0x2F,0x81);//Serial Font ROM Setting GT23L32S4W normal
			Write_Dir(0x22,0xC5);//Full alignment is enable.The text background color
			FontWrite_Position(400,210);//Text written to the position
		    Write_Dir(0x40,0x80);//Set the character mode
		    LCD_CmdWrite(0x02);//start write data
		    String("20.000");			
			HAL_Delay(1000);	
			for(int n=1; n<8;n++)
			{
				Displaypicture(n);
				HAL_Delay(20);
			}			
}

void test_lc(void)
{
		Graphic_Mode();
    Active_Window(0,799,0,479);
		LCD_background(0x07E0);
		Memory_Clear();
		HAL_Delay(1000);
//	Text_Background_Color(0,0,255);
//		Memory_Clear();
//		HAL_Delay(1000);
//	Clear_Active_Window();
//	Text_Background_Color(0,0,0);
//	Memory_Clear();
//	Graphic_Mode();
//	LCD_Clear(0x0FFF);
	
	Draw_Circle(200,240,100);	
	drawBITMAP();

//	LCD_Clear(0xFFF0);

}

void LCD_PLL_ini(void)
{
    LCD_CmdWrite(0x88);      
    LCD_DataWrite(0x0C);
    HAL_Delay(1);    
    LCD_CmdWrite(0x89);        
    LCD_DataWrite(0x01);  
    HAL_Delay(1);
}

void LCD_TFT_Init(void)
{
	LCD_PLL_ini();
	LCD_CmdWrite(0x10);  //SYSR   bit[4:3]=00 256 color  bit[2:1]=  00 8bit MPU interface
  LCD_DataWrite(0x03);   // if 8bit MCU interface   and 256 color display						    
	HAL_Delay(1);
	
LCD_CmdWrite(0x04);  //PCLK inverse
 LCD_DataWrite(0x81);		//pixel = sys/2
 HAL_Delay(1);
 
 //Horizontal set
 LCD_CmdWrite(0x14); //HDWR//Horizontal Display Width Setting Bit[6:0]                      
 LCD_DataWrite(0x63);//Horizontal display width(pixels) = (HDWR + 1)*8                      
 LCD_CmdWrite(0x15);//Horizontal Non-Display Period Fine Tuning Option Register (HNDFTR)   
 LCD_DataWrite(0x00);//Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]               
 LCD_CmdWrite(0x16); //HNDR//Horizontal Non-Display Period Bit[4:0]                         
 LCD_DataWrite(0x03);//Horizontal Non-Display Period (pixels) = (HNDR + 1)*8                
 LCD_CmdWrite(0x17); //HSTR//HSYNC Start Position[4:0]                                      
 LCD_DataWrite(0x03);//HSYNC Start Position(PCLK) = (HSTR + 1)*8                            
 LCD_CmdWrite(0x18); //HPWR//HSYNC Polarity ,The period width of HSYNC.                     
 LCD_DataWrite(0x0B);//HSYNC Width [4:0]   HSYNC Pulse width(PCLK) = (HPWR + 1)*8    
 //Vertical set                             
 LCD_CmdWrite(0x19); //VDHR0 //Vertical Display Height Bit [7:0]                            
 LCD_DataWrite(0xdf);//Vertical pixels = VDHR + 1                                           
 LCD_CmdWrite(0x1a); //VDHR1 //Vertical Display Height Bit [8]                              
 LCD_DataWrite(0x01);//Vertical pixels = VDHR + 1                                           
 LCD_CmdWrite(0x1b); //VNDR0 //Vertical Non-Display Period Bit [7:0]                        
 LCD_DataWrite(0x20);//Vertical Non-Display area = (VNDR + 1)                               
 LCD_CmdWrite(0x1c); //VNDR1 //Vertical Non-Display Period Bit [8]                          
 LCD_DataWrite(0x00);//Vertical Non-Display area = (VNDR + 1)                               
 LCD_CmdWrite(0x1d); //VSTR0 //VSYNC Start Position[7:0]                                    
 LCD_DataWrite(0x16);//VSYNC Start Position(PCLK) = (VSTR + 1)                              
 LCD_CmdWrite(0x1e); //VSTR1 //VSYNC Start Position[8]                                      
 LCD_DataWrite(0x00);//VSYNC Start Position(PCLK) = (VSTR + 1)                              
 LCD_CmdWrite(0x1f); //VPWR //VSYNC Polarity ,VSYNC Pulse Width[6:0]                        
 LCD_DataWrite(0x01);//VSYNC Pulse Width(PCLK) = (VPWR + 1)                                 
  
  //Active window  set 
   //setting active window X
    LCD_CmdWrite(0x30); //Horizontal Start Point 0 of Active Window (HSAW0)
    LCD_DataWrite(0x00); //Horizontal Start Point of Active Window [7:0]
    LCD_CmdWrite(0x31); //Horizontal Start Point 1 of Active Window (HSAW1)	   
    LCD_DataWrite(0x00); //Horizontal Start Point of Active Window [9:8]
    LCD_CmdWrite(0x34); //Horizontal End Point 0 of Active Window (HEAW0)
    LCD_DataWrite(0x1F); //Horizontal End Point of Active Window [7:0]
    LCD_CmdWrite(0x35); //Horizontal End Point 1 of Active Window (HEAW1)	   
    LCD_DataWrite(0x03); //Horizontal End Point of Active Window [9:8]

   //setting active window Y
    LCD_CmdWrite(0x32); //Vertical Start Point 0 of Active Window (VSAW0)
    LCD_DataWrite(0x00); //Vertical Start Point of Active Window [7:0]
    LCD_CmdWrite(0x33); //Vertical Start Point 1 of Active Window (VSAW1)	   
    LCD_DataWrite(0x00); //Vertical Start Point of Active Window [8]
    LCD_CmdWrite(0x36); //Vertical End Point of Active Window 0 (VEAW0)
    LCD_DataWrite(0xdf); //Vertical End Point of Active Window [7:0]
    LCD_CmdWrite(0x37); //Vertical End Point of Active Window 1 (VEAW1)	   
    LCD_DataWrite(0x01); //Vertical End Point of Active Window [8]

//		LCD_CmdWrite(0x20);
//		LCD_DataWrite(0x00);
		
		LCD_background(0x0000);
		
//		LCD_CmdWrite(0x8a);//PWM setting
//	  LCD_DataWrite(0x80);
		
		LCD_CmdWrite(0x8a);//PWM setting
		LCD_DataWrite(0x87);//open PWM
		
		LCD_CmdWrite(0x8b);//Backlight brightness setting
		LCD_DataWrite(20);//Brightness parameter 0xff-0x00
			printf("LCD_COLOR--\r\n");
		Display_ON();
		Memory_Clear();
//		test_lcd();
}

void Display_ON(void)
{	
  LCD_CmdWrite(0x01);//PWRR
	LCD_DataWrite(0x80);
}

void Display_OFF(void)
{
	LCD_CmdWrite(0x01);//PWRR
	LCD_DataWrite(0x00);
}



//Cor de fundo com 65k colors
void LCD_background(uint16_t color)
{
	LCD_CmdWrite(0x60);
	LCD_DataWrite((color&0xf800)>>11);
  LCD_CmdWrite(0x61);
	LCD_DataWrite((color&0x07e0)>>5);
	LCD_CmdWrite(0x62);
	LCD_DataWrite((color&0x001f));
}

//#endif //end lcd_color
