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
�G���̺޸}���� - �L�D�N���
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


�G���̺޸}���� - ���D�N���
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
* �W    �١Jvoid ili9320_Initializtion()
* �\    ��J��l�� ILI9320 ���
* �J�f�ѼơJ�L
* �X�f�ѼơJ�L
* ��    ���J
* �եΤ�k�Jili9320_Initializtion();
****************************************************************************/
void ili9320_Initializtion()
{ 
   LCD_Init1();
}

/****************************************************************************
* �W    �١Jvoid ili9320_SetCursor(u16 x,u16 y)
* �\    ��J�]�m�̹��y��
* �J�f�ѼơJx      ��y��
*           y      �C�y��
* �X�f�ѼơJ�L
* ��    ���J
* �եΤ�k�Jili9320_SetCursor(10,10);
****************************************************************************/
void ili9320_SetCursor(u16 x,u16 y)
{				
  	LCD_WR_CMD(0x5F,x);
	LCD_WR_CMD(0x60,x>>8);	
	LCD_WR_CMD(0x61,y);
	LCD_WR_CMD(0x62,y>>8);
}

/****************************************************************************
* �W    �١Jvoid ili9320_SetWindows(u16 StartX,u16 StartY,u16 
EndX,u16 EndY)
* �\    ��J�]�m���f�ϰ�
* �J�f�ѼơJStartX     ��_�l�y��
*           StartY     �C�_�l�y��
*           EndX       �浲���y��
*           EndY       �C�����y��
* �X�f�ѼơJ�L
* ��    ���J
* �եΤ�k�Jili9320_SetWindows(0,0,100,100)�F
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
* ��    �ƣ�u16 ili9320_GetPoint(u16 x,u16 y)
* ��    �ܣ���ȡָ���������ɫֵ
* ��ڲ�����x      ������
*           y      ������
* ���ڲ�������ǰ������ɫֵ
* ˵    ����
* ���÷�����i=ili9320_GetPoint(10,10);
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
* ��    �ƣ�void ili9320_SetPoint(u16 x,u16 y,u16 point)
* ��    �ܣ���ָ�����껭��
* ��ڲ�����x      ������
*           y      ������
*           point  �����ɫ
* ���ڲ�������
* ˵    ����
* ���÷�����ili9320_SetPoint(10,10,0x0fe0);
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
* ��    �ƣ�void ili9320_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
* ��    �ܣ���ָ�����귶Χ��ʾһ��ͼƬ
* ��ڲ�����StartX     ����ʼ����
*           StartY     ����ʼ����
*           EndX       �н�������
*           EndY       �н�������
            pic        ͼƬͷָ��
* ���ڲ�������
* ˵    ����ͼƬȡģ��ʽΪˮƽɨ�裬16λ��ɫģʽ
* ���÷�����ili9320_DrawPicture(0,0,100,100,(u16*)demo);
****************************************************************************/
void ili9320_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
{
  u16  i;
  ili9320_SetWindows(StartX,StartY,EndX,EndY);
  ili9320_SetCursor(StartX,StartY);	    
  for (i=0;i<(EndX*EndY);i++) LCD_WR_Data(*pic++);
}

/****************************************************************************
* ��    �ƣ�void ili9320_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
* ��    �ܣ���ָ��������ʾһ��8x16�����ascii�ַ�
* ��ڲ�����x          ������
*           y          ������
*           charColor  �ַ�����ɫ
*           bkColor    �ַ�������ɫ
* ���ڲ�������
* ˵    ������ʾ��Χ�޶�Ϊ����ʾ��ascii��
* ���÷�����ili9320_PutChar(10,10,'a',0x0000,0xffff);
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
          ili9320_SetPoint(x+j,y+i,charColor); // �ַ���ɫ
        }
        else
        {
          ili9320_SetPoint(x+j,y+i,bkColor); // ������ɫ
        }
    }
  }
}

/****************************************************************************
* ��    �ƣ�void ili9320_Test()
* ��    �ܣ�����Һ����
* ��ڲ�������
* ���ڲ�������
* ˵    ������ʾ����������Һ�����Ƿ���������
* ���÷�����ili9320_Test();
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
* ��    �ƣ�u16 ili9320_BGR2RGB(u16 c)
* ��    �ܣ�RRRRRGGGGGGBBBBB ��Ϊ BBBBBGGGGGGRRRRR ��ʽ
* ��ڲ�����c      BRG ��ɫֵ
* ���ڲ�����RGB ��ɫֵ
* ˵    �����ڲ���������
* ���÷�����
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
* ��    �ƣ�void ili9320_WriteIndex(u16 idx)
* ��    �ܣ�д ili9320 �������Ĵ�����ַ
* ��ڲ�����idx   �Ĵ�����ַ
* ���ڲ�������
* ˵    ��������ǰ����ѡ�п��������ڲ�����
* ���÷�����ili9320_WriteIndex(0x0000);
****************************************************************************/
void ili9320_WriteIndex(u16 idx)
{
  LCD_WR_REG(idx);
}

/****************************************************************************
* ��    �ƣ�void ili9320_WriteData(u16 dat)
* ��    �ܣ�д ili9320 �Ĵ�������
* ��ڲ�����dat     �Ĵ�������
* ���ڲ�������
* ˵    �����������ָ����ַд�����ݣ�����ǰ����д�Ĵ�����ַ���ڲ�����
* ���÷�����ili9320_WriteData(0x1030)
****************************************************************************/
void ili9320_WriteData(u16 dat)
{
  LCD_WR_Data(dat);
}

/****************************************************************************
* ��    �ƣ�u16 ili9320_ReadData(void)
* ��    �ܣ���ȡ����������
* ��ڲ�������
* ���ڲ��������ض�ȡ��������
* ˵    �����ڲ�����
* ���÷�����i=ili9320_ReadData();
****************************************************************************/
u16 ili9320_ReadData(void)
{
  u16 val=0;
  val=LCD_RD_data();
  return val;
}

/****************************************************************************
* ��    �ƣ�u16 ili9320_ReadRegister(u16 index)
* ��    �ܣ���ȡָ����ַ�Ĵ�����ֵ
* ��ڲ�����index    �Ĵ�����ַ
* ���ڲ������Ĵ���ֵ
* ˵    �����ڲ�����
* ���÷�����i=ili9320_ReadRegister(0x0022);
****************************************************************************/
u16 ili9320_ReadRegister(u16 index)
{
  u16 tmp;
  tmp= *(volatile unsigned int *)(0x60000000);
  
  return tmp;
}

/****************************************************************************
* ��    �ƣ�void ili9320_WriteRegister(u16 index,u16 dat)
* ��    �ܣ�дָ����ַ�Ĵ�����ֵ
* ��ڲ�����index    �Ĵ�����ַ
*         ��dat      �Ĵ���ֵ
* ���ڲ�������
* ˵    �����ڲ�����
* ���÷�����ili9320_WriteRegister(0x0000,0x0001);
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
* ��    �ƣ�void ili9320_Reset()
* ��    �ܣ���λ ili9320 ������
* ��ڲ�������
* ���ڲ�������
* ˵    ������λ���������ڲ�����
* ���÷�����ili9320_Reset()
****************************************************************************/
void ili9320_Reset()
{

}

/****************************************************************************
* ��    �ƣ�void ili9320_BackLight(u8 status)
* ��    �ܣ�������Һ������
* ��ڲ�����status     1:���⿪  0:�����
* ���ڲ�������
* ˵    ����
* ���÷�����ili9320_BackLight(1);
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
* ��    �ƣ�void ili9320_Delay(vu32 nCount)
* ��    �ܣ���ʱ
* ��ڲ�����nCount   ��ʱֵ
* ���ڲ�������
* ˵    ����
* ���÷�����ili9320_Delay(10000);
****************************************************************************/
void ili9320_Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}