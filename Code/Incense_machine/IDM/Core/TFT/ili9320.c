#include "stm32f10x.h"
#include "ili9320.h"
#include "ili9320_font.h"
#include "lcdconf.h"


extern void LCD_WR_REG(unsigned char index);
extern void LCD_WR_CMD(unsigned int index,unsigned int val);

extern void LCD_WR_Data(unsigned int val);
extern void LCD_WR_Data_8(unsigned char val);
extern void LCD_test(void);
extern void LCD_clear(unsigned int p);

extern void lcd_wr_zf(unsigned int a, unsigned int b, unsigned int a1,unsigned int b1, unsigned int d,unsigned int e, unsigned char g, unsigned char *f); 
extern void lcd_wr_pixel(unsigned int a, unsigned int b, unsigned int e) ;
extern unsigned char *num_pub(unsigned int a);
extern unsigned int LCD_RD_data(void);


/*
for stmfans-ev board
lcd:
PD0 -> nCS  PD1 -> RS  PD2 -> nWR  PD3 -> nRD  PD4 -> nRESET
touch:
PD5 -> TS_CLK  PD6 -> TS_nCS  PD7 -> TS_DIN  PD8 -> TS_BUSY  PD9 -> TS_nPENIRQ  PD10 -> TS_DOUT
*/
#define nCS     GPIO_Pin_0
#define RS      GPIO_Pin_1
#define nWR     GPIO_Pin_2
#define nRD     GPIO_Pin_3
#define nReset  GPIO_Pin_4
//#define LCD_BK  GPIO_Pin_0

/*
睪垂恨竲だガ - 礚郉篘北竟
 1 - DB0      2 - DB1      3 - DB2      4 - DB3      5 - DB4      6 - DB5      7 - 
DB6      8 - DB7
 9 - DB8     10 - DB9     11 - DB10    12 - DB11    13 - DB12    14 - DB13    
15 - DB14    16 - DB15
17 - nRD     18 - RS      19 - nWR     20 - nCS     21 - NC      22 - nReset  
23 - VCC     24 - VCC
25 - VCC     26 - VCC     27 - NC      28 - NC      29 - GND     30 - GND     
31 - BK1     32 - BK2
33 - BK3     34 - BK4     35 - GND     36 - GND     37 - X-      38 - Y-      39 - 
X+      40 - Y+


睪垂恨竲だガ - Τ郉篘北竟
 1 - DB0      2 - DB1      3 - DB2      4 - DB3      5 - DB4      6 - DB5      7 - 
DB6      8 - DB7
 9 - DB8     10 - DB9     11 - DB10    12 - DB11    13 - DB12    14 - DB13    
15 - DB14    16 - DB15
17 - nCS     18 - RS      19 - nWR     20 - NC      21 - nRD     22 - nReset  
23 - VCC     24 - VCC
25 - VCC     26 - VCC     27 - NC      28 - BL      29 - GND     30 - GND     31 
- GND     32 - GND
33 - TS_CLK  34 - TS_VBAT 35 - TS_nCS  36 - TS_AUX  37 - TS_DIN  38 
- TS_DOUT 39 - TS_BUSY 40 - TS_nPENIRQ
*/



/****************************************************************************
*     嘿void ili9320_Initializtion()
*     ﹍て ILI9320 北竟
* 把计礚
* 把计礚
* 弧    
* 秸ノよ猭ili9320_Initializtion();
****************************************************************************/
void ili9320_Initializtion()
{ 
   LCD_Init1();
}

/****************************************************************************
*     嘿void ili9320_SetCursor(u16 x,u16 y)
*     砞竚辊畒夹
* 把计x      ︽畒夹
*           y      畒夹
* 把计礚
* 弧    
* 秸ノよ猭ili9320_SetCursor(10,10);
****************************************************************************/
void ili9320_SetCursor(u16 x,u16 y)
{				
  	LCD_WR_CMD(0x5F,x);
	LCD_WR_CMD(0x60,x>>8);	
	LCD_WR_CMD(0x61,y);
	LCD_WR_CMD(0x62,y>>8);
}

/****************************************************************************
*     嘿void ili9320_SetWindows(u16 StartX,u16 StartY,u16 
EndX,u16 EndY)
*     砞竚怠跋办
* 把计StartX     ︽癬﹍畒夹
*           StartY     癬﹍畒夹
*           EndX       ︽挡畒夹
*           EndY       挡畒夹
* 把计礚
* 弧    
* 秸ノよ猭ili9320_SetWindows(0,0,100,100)
****************************************************************************/
void ili9320_SetWindows(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
{
  	LCD_WR_CMD(0x56,StartX);
	LCD_WR_CMD(0x57,StartX>>8);	
	LCD_WR_CMD(0x58,StartY);
	LCD_WR_CMD(0x59,StartY>>8);

	LCD_RegisterWrite(0x5A,(EndX-StartX+1));
	LCD_RegisterWrite(0x5B,(EndX-StartX+1)>>8);
 
 	LCD_RegisterWrite(0x5C,(EndY-StartY+1));
	LCD_RegisterWrite(0x5D,(EndY-StartY+1)>>8);
  
}



/****************************************************************************
* 名    称：u16 ili9320_GetPoint(u16 x,u16 y)
* 功    能：获取指定座标的颜色值
* 入口参数：x      行座标
*           y      列座标
* 出口参数：当前座标颜色值
* 说    明：
* 调用方法：i=ili9320_GetPoint(10,10);
****************************************************************************/
u16 ili9320_GetPoint(u16 x,u16 y)
{ u16 temp;
  ili9320_SetCursor(x,y);
  LCD_WR_REG(0x04);
  //temp = ili9320_ReadData(); //dummy
  //temp = ili9320_ReadData(); 	

  return (ili9320_BGR2RGB(ili9320_ReadData()));
  //return (ili9320_ReadData());
}

/****************************************************************************
* 名    称：void ili9320_SetPoint(u16 x,u16 y,u16 point)
* 功    能：在指定座标画点
* 入口参数：x      行座标
*           y      列座标
*           point  点的颜色
* 出口参数：无
* 说    明：
* 调用方法：ili9320_SetPoint(10,10,0x0fe0);
****************************************************************************/
void ili9320_SetPoint(u16 x,u16 y,u16 point)
{
  //if ( (x>240)||(y>320) ) return;
  //LCD_WR_CMD(0,0x02,x);
  //LCD_WR_CMD(1,0x03,y);  
  
 
  LCD_WR_CMD(32, y);
  LCD_WR_CMD(33, 319-x);
		//*(__IO uint16_t *) (Bank1_LCD_C)= 34;
  LCD_WR_REG(34);

  //LCD_WR_CMD(0,0x02,y);
  //LCD_WR_CMD(1,0x03,319-x);  
  //LCD_WR_REG(0x0E); 
  LCD_WR_Data(point);  
}

/****************************************************************************
* 名    称：void ili9320_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
* 功    能：在指定座标范围显示一副图片
* 入口参数：StartX     行起始座标
*           StartY     列起始座标
*           EndX       行结束座标
*           EndY       列结束座标
            pic        图片头指针
* 出口参数：无
* 说    明：图片取模格式为水平扫描，16位颜色模式
* 调用方法：ili9320_DrawPicture(0,0,100,100,(u16*)demo);
****************************************************************************/
void ili9320_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
{
  u16  i;
  ili9320_SetWindows(StartX,StartY,EndX,EndY);
  ili9320_SetCursor(StartX,StartY);	    
  for (i=0;i<(EndX*EndY);i++) LCD_WR_Data(*pic++);
}

/****************************************************************************
* 名    称：void ili9320_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
* 功    能：在指定座标显示一个8x16点阵的ascii字符
* 入口参数：x          行座标
*           y          列座标
*           charColor  字符的颜色
*           bkColor    字符背景颜色
* 出口参数：无
* 说    明：显示范围限定为可显示的ascii码
* 调用方法：ili9320_PutChar(10,10,'a',0x0000,0xffff);
****************************************************************************/
void ili9320_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
{
  u16 i=0;
  u16 j=0;
  
  u8 tmp_char=0;

  for (i=0;i<16;i++)
  {
    tmp_char=ascii_8x16[((c-0x20)*16)+i];
    for (j=0;j<8;j++)
    {
      if ( (tmp_char >> 7-j) & 0x01 == 0x01)
        {
          ili9320_SetPoint(x+j,y+i,charColor); // 字符颜色
        }
        else
        {
          ili9320_SetPoint(x+j,y+i,bkColor); // 背景颜色
        }
    }
  }
}

/****************************************************************************
* 名    称：void ili9320_Test()
* 功    能：测试液晶屏
* 入口参数：无
* 出口参数：无
* 说    明：显示彩条，测试液晶屏是否正常工作
* 调用方法：ili9320_Test();
****************************************************************************/
void ili9320_Test()
{
  u16 i,j;
  ili9320_SetCursor(0,0);
  
  for(i=0;i<320;i++)
    for(j=0;j<240;j++)
    {
      if(i>279)LCD_WR_Data(0x0000);
      else if(i>239)LCD_WR_Data(0x001f);
      else if(i>199)LCD_WR_Data(0x07e0);
      else if(i>159)LCD_WR_Data(0x07ff);
      else if(i>119)LCD_WR_Data(0xf800);
      else if(i>79)LCD_WR_Data(0xf81f);
      else if(i>39)LCD_WR_Data(0xffe0);
      else LCD_WR_Data(0xffff);
    }
  
}

/****************************************************************************
* 名    称：u16 ili9320_BGR2RGB(u16 c)
* 功    能：RRRRRGGGGGGBBBBB 改为 BBBBBGGGGGGRRRRR 格式
* 入口参数：c      BRG 颜色值
* 出口参数：RGB 颜色值
* 说    明：内部函数调用
* 调用方法：
****************************************************************************/
u16 ili9320_BGR2RGB(u16 c)
{
  u16  r, g, b;

  b = (c>>0)  & 0x1f;
  g = (c>>5)  & 0x3f;
  r = (c>>11) & 0x1f;
  
  return( (b<<11) + (g<<5) + (r<<0) );
}

/****************************************************************************
* 名    称：void ili9320_WriteIndex(u16 idx)
* 功    能：写 ili9320 控制器寄存器地址
* 入口参数：idx   寄存器地址
* 出口参数：无
* 说    明：调用前需先选中控制器，内部函数
* 调用方法：ili9320_WriteIndex(0x0000);
****************************************************************************/
void ili9320_WriteIndex(u16 idx)
{
  LCD_WR_REG(idx);
}

/****************************************************************************
* 名    称：void ili9320_WriteData(u16 dat)
* 功    能：写 ili9320 寄存器数据
* 入口参数：dat     寄存器数据
* 出口参数：无
* 说    明：向控制器指定地址写入数据，调用前需先写寄存器地址，内部函数
* 调用方法：ili9320_WriteData(0x1030)
****************************************************************************/
void ili9320_WriteData(u16 dat)
{
  LCD_WR_Data(dat);
}

/****************************************************************************
* 名    称：u16 ili9320_ReadData(void)
* 功    能：读取控制器数据
* 入口参数：无
* 出口参数：返回读取到的数据
* 说    明：内部函数
* 调用方法：i=ili9320_ReadData();
****************************************************************************/
u16 ili9320_ReadData(void)
{
  u16 val=0;
  val=LCD_RD_data();
  return val;
}

/****************************************************************************
* 名    称：u16 ili9320_ReadRegister(u16 index)
* 功    能：读取指定地址寄存器的值
* 入口参数：index    寄存器地址
* 出口参数：寄存器值
* 说    明：内部函数
* 调用方法：i=ili9320_ReadRegister(0x0022);
****************************************************************************/
u16 ili9320_ReadRegister(u16 index)
{
  u16 tmp;
  tmp= *(volatile unsigned int *)(0x60000000);
  
  return tmp;
}

/****************************************************************************
* 名    称：void ili9320_WriteRegister(u16 index,u16 dat)
* 功    能：写指定地址寄存器的值
* 入口参数：index    寄存器地址
*         ：dat      寄存器值
* 出口参数：无
* 说    明：内部函数
* 调用方法：ili9320_WriteRegister(0x0000,0x0001);
****************************************************************************/
void ili9320_WriteRegister(u16 index,u16 dat)
{
 /************************************************************************
  **                                                                    **
  ** nCS       ----\__________________________________________/-------  **
  ** RS        ------\____________/-----------------------------------  **
  ** nRD       -------------------------------------------------------  **
  ** nWR       --------\_______/--------\_____/-----------------------  **
  ** DB[0:15]  ---------[index]----------[data]-----------------------  **
  **                                                                    **
  ************************************************************************/
  
  LCD_WR_CMD(index,dat);
}

/****************************************************************************
* 名    称：void ili9320_Reset()
* 功    能：复位 ili9320 控制器
* 入口参数：无
* 出口参数：无
* 说    明：复位控制器，内部函数
* 调用方法：ili9320_Reset()
****************************************************************************/
void ili9320_Reset()
{

}

/****************************************************************************
* 名    称：void ili9320_BackLight(u8 status)
* 功    能：开、关液晶背光
* 入口参数：status     1:背光开  0:背光关
* 出口参数：无
* 说    明：
* 调用方法：ili9320_BackLight(1);
****************************************************************************/
void ili9320_BackLight(u8 status)
{
  if ( status >= 1 )
  {
    //GPIO_SetBits(GPIOC,LCD_BK);
  }
  else
  {
    //GPIO_ResetBits(GPIOC,LCD_BK);
  }
}

/****************************************************************************
* 名    称：void ili9320_Delay(vu32 nCount)
* 功    能：延时
* 入口参数：nCount   延时值
* 出口参数：无
* 说    明：
* 调用方法：ili9320_Delay(10000);
****************************************************************************/
void ili9320_Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}