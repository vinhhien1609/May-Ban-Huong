/* Includes ------------------------------------------------------------------*/
//#include "fsmc_sram.h"
#include "stdlib.h"
//#include "stm32f10x_tim.h"
#include "UserDef.h"
#include "RA_lcd.h"
#include "fonts.h"
#include "RA8875.h"
#include "TFT_glcd.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/                              
/* Private variables ---------------------------------------------------------*/
/* Global variables to set the written text color */
static  u16 TextColor = 0x0000, BackColor = 0xFFFF;
  
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


//------------------------------------
/**/
void LCD_Delay(unsigned long nCount)
{
  for(; nCount != 0; nCount--);
}

void RA8875_HW_Reset(void)
{
	LCD_Delay(5000000);		 //wait ra8875 power on 
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
	LCD_Delay(256000);					   
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);		 	 
	LCD_Delay(100000);	
}


void lcd_rst(void)
{
		HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
    LCD_Delay(100000);					   
    HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);		 	 
    LCD_Delay(100000);	
}


void LCD_WriteRAM_Prepare(void)
{
  LCD_CmdWrite(0x02); //
}

/*******************************************************************************
* Function Name  : LCD_SetTextColor
* Description    : Sets the Text color.
* Input          : - Color: specifies the Text color code RGB(5-6-5).
* Output         : - TextColor: Text color global variable used by LCD_DrawChar
*                  and LCD_DrawPicture functions.
* Return         : None
*******************************************************************************/
void LCD_SetTextColor(u16 Color)
{
  TextColor = Color;
}

/*******************************************************************************
* Function Name  : LCD_SetBackColor
* Description    : Sets the Background color.
* Input          : - Color: specifies the Background color code RGB(5-6-5).
* Output         : - BackColor: Background color global variable used by 
*                  LCD_DrawChar and LCD_DrawPicture functions.
* Return         : None
*******************************************************************************/
void LCD_SetBackColor(u16 Color)
{
  BackColor = Color;
}


/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : Clears the hole LCD.
* Input          : Color: the color of the background.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Clear(u16 Color)
{
  u32 index = 0;
  
  LCD_SetCursor(0,0); 

  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

  for(index = 0; index < 480000; index++)
  {
    LCD_DataWrite(Color);

  }

 // while(1);  
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
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

void LCD_SetReadCursor(u16 Xpos, u16 Ypos)
{
    LCD_CmdWrite(0x4a);
    LCD_DataWrite(Xpos);  
    LCD_CmdWrite(0x4b);	   
    LCD_DataWrite(Xpos>>8);
    LCD_CmdWrite(0x4c);
    LCD_DataWrite(Ypos);
    LCD_CmdWrite(0x4d);	   
    LCD_DataWrite(Ypos>>8);
}

u16 LCD_GetPoint(u16 x,u16 y)
{ u16 temp;
  LCD_SetReadCursor(x,y);
  LCD_WriteRAM_Prepare();
  
  temp=LCD_DataRead();//Dummy
   __nop();
   __nop();
   __nop();
   __nop();
  temp=LCD_DataRead();
  return temp;
}
		 
void LCD_SetPoint(u16 x,u16 y,u16 point)
{
  LCD_SetCursor(x,y);
  LCD_WriteRAM_Prepare();
  LCD_DataWrite(point);

}


void LCD_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
{
  u16 i=0;
  u16 j=0;
  
  u16 tmp_char=0;

  for (i=0;i<24;i++)
  {
    tmp_char=ASCII_Table[((c-0x20)*24)+i];	 //��h32�������A�O�]���r�Ū�q�Ů�}�l���A�ѦҦr�Ū����`��
    for (j=0;j<16;j++)
    {
//      if ( (tmp_char >> 15-j) & 0x01 == 0x01)	���ӤW������ܡA�r�ŬO�˹L�Ӫ��A��o����L�ӴN��F
	if ( (tmp_char >> j) & 0x01 == 0x01)
      {
        LCD_SetPoint(x+j,y+i,charColor); //�r���C��
      }
      else
      {
        LCD_SetPoint(x+j,y+i,bkColor); // �r���C��
      }
    }
  }
}


void LCD_DisplayString(u16 X,u16 Y, char *ptr, u16 charColor, u16 bkColor)
{
  u32 i = 0;

  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (i < 50))
  {
    /* Display one character on LCD */
	LCD_PutChar(X, Y, *ptr, charColor, bkColor);
    /* Decrement the column position by 16 */
    X += 16;
    /* Point on the next character */
    ptr++;
    /* Increment the character counter */
    i++;
  }
}



/*******************************************************************************
* Function Name  : LCD_DisplayStringLine
* Description    : Displays a maximum of 20 char on the LCD.
* Input          : - Line: the Line where to display the character shape .
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..9
*                  - *ptr: pointer to string to display on LCD.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayStringLine(u16 Line, char *ptr, u16 charColor, u16 bkColor)
{
  u32 i = 0;
  u16 refcolumn = 0;

  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (i < 50))
  {
    /* Display one character on LCD */
//    LCD_PutChar(refcolumn, Line, *ptr, charColor, bkColor);
	LCD_PutChar(refcolumn, Line, *ptr, charColor, bkColor);
    /* Decrement the column position by 16 */
    refcolumn += 16;
    /* Point on the next character */
    ptr++;
    /* Increment the character counter */
    i++;
  }
}

/*******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : Displays a line.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
*                  - Length: line length.
*                  - Direction: line direction.
*                    This parameter can be one of the following values: Vertical 
*                    or Horizontal.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawLine(u16 Xpos, u16 Ypos, u16 Length, u8 Direction)
{
  u32 i = 0;
  
  LCD_SetCursor(Xpos,319 - Ypos);

  if(Direction == Horizontal)
  {
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    for(i = 0; i < Length; i++)
    {
      LCD_DataWrite(TextColor);
    }
  }
  else
  {
    for(i = 0; i < Length; i++)
    {
      LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
      LCD_DataWrite(TextColor);
      Xpos++;
      LCD_SetCursor(Xpos, 479 - Ypos);
    }
  }
}


/*******************************************************************************
* Function Name  : LCD_DrawRect
* Description    : Displays a rectangle.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
*                  - Height: display rectangle height.
*                  - Width: display rectangle width.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawRect(u16 Xpos, u16 Ypos, u16 Height, u16 Width)
{
  LCD_DrawLine(Xpos, Ypos, Width, Horizontal);
  LCD_DrawLine((Xpos + Height), Ypos, Width, Horizontal);
  
  LCD_DrawLine(Xpos, Ypos, Height, Vertical);
  LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, Vertical);
}

/*******************************************************************************
* Function Name  : LCD_DrawCircle
* Description    : Displays a circle.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
*                  - Height: display rectangle height.
*                  - Width: display rectangle width.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawCircle(u16 Xpos, u16 Ypos, u16 Radius)
{
  s32  D;/* Decision Variable */ 
  u32  CurX;/* Current X Value */
  u32  CurY;/* Current Y Value */ 
  
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;
  
  while (CurX <= CurY)
  {
    LCD_SetCursor(Xpos + CurX, Ypos + CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_DataWrite(TextColor);

    LCD_SetCursor(Xpos + CurX, Ypos - CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_DataWrite(TextColor);

    LCD_SetCursor(Xpos - CurX, Ypos + CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_DataWrite(TextColor);

    LCD_SetCursor(Xpos - CurX, Ypos - CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_DataWrite(TextColor);

    LCD_SetCursor(Xpos + CurY, Ypos + CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_DataWrite(TextColor);

    LCD_SetCursor(Xpos + CurY, Ypos - CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_DataWrite(TextColor);

    LCD_SetCursor(Xpos - CurY, Ypos + CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_DataWrite(TextColor);

    LCD_SetCursor(Xpos - CurY, Ypos - CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_DataWrite(TextColor);

    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}


////////////Show the picture of the flash
void Displaypicture(uint8_t picnum)
{  
   uint8_t picnumtemp;
//	Read_flash(1);
   Active_Window(0,799,0,479); 
   XY_Coordinate(0,0);//Memory write position	
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




/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/


