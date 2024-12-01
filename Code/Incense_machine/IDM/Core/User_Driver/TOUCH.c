#include "TOUCH.h"

#include "gslx680.h"
#include "app_debug.h"


struct _ts_event ts_event; 

#define SDA_read	HAL_GPIO_ReadPin(TOUCH_SDA_GPIO_Port, TOUCH_SDA_Pin)

void SDA_input()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = TOUCH_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TOUCH_SDA_GPIO_Port, &GPIO_InitStruct);
}
void SDA_out()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = TOUCH_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TOUCH_SDA_GPIO_Port, &GPIO_InitStruct);
}
void SCL_out()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = TOUCH_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TOUCH_SCL_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = TOUCH_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TOUCH_RST_GPIO_Port, &GPIO_InitStruct);	
	DEBUG_INFO("SCL_1\r\n");
}

void WAKE(unsigned char x)
{
	if(x)	HAL_GPIO_WritePin(TOUCH_RST_GPIO_Port, TOUCH_RST_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(TOUCH_RST_GPIO_Port, TOUCH_RST_Pin, GPIO_PIN_RESET);
}

void SDA(unsigned char x)
{
	if(x)	HAL_GPIO_WritePin(TOUCH_SDA_GPIO_Port, TOUCH_SDA_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(TOUCH_SDA_GPIO_Port, TOUCH_SDA_Pin, GPIO_PIN_RESET);
}

void SCL(unsigned char x)
{
	if(x)	HAL_GPIO_WritePin(TOUCH_SCL_GPIO_Port, TOUCH_SCL_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(TOUCH_SCL_GPIO_Port, TOUCH_SCL_Pin, GPIO_PIN_RESET);
}


void _nop_()
{
	uint16_t n=3;
	while(n--);
}
void delayus(uint16_t time)
{
	uint16_t n= time;
	while (n--)
	{
		_nop_();
	}
}

//IIC start
void TOUCH_Start(void)
{
	SDA_out();
	SDA(1);  
	_nop_();	  
	SCL(1);
	delayus(5);
	SDA(0);  
	delayus(5);
	SCL(0);
	_nop_();
}	  


//IIC stop
void TOUCH_Stop(void)
{
	SDA_out();
	SDA(0);
	_nop_();
	SCL(1);
	delayus(5);
	SDA(1);
	delayus(5);
	SCL(0);
	_nop_();
}

/*
//Wait for an answer signal
unsigned char TOUCH_Wait_Ack(void)
{
    unsigned char ack=1;
	SDA=1;
    SCL=1;
	delayus(2);
    while(ack==1){ack=SDA;}
	delayus(2);
    SCL=0;
    return 0;

}*/
//Wait for an answer signal
unsigned char TOUCH_Wait_Ack(void)
{	unsigned char errtime=0;

	SDA_out();
	SDA(1);
	delayus(1);	
	SCL(1);
	SDA_input();	
//	delayus(1);
  	while(SDA_read)
	{
	    errtime++;
	    if(errtime>50)
		    {
		      TOUCH_Stop();
		      return 1;
		    }
	}
	SCL(0);
	_nop_();
}



//Acknowledge
void TOUCH_Ack(void)
{	
	SDA_out();	
	SCL(0);
	SDA(0);
	delayus(2);
	SCL(1);
	delayus(2);
	SCL(0);
	_nop_();
}



//NO Acknowledge		    
void TOUCH_NAck(void)
{	
	SDA_out();		
	SCL(0);
	SDA(1);
	delayus(2);
	SCL(1);
	delayus(2);
	SCL(0);
	_nop_();
}	
	

//IIC send one byte	
void TOUCH_Send_Byte(unsigned char Byte)
{	unsigned char t;  		
	SDA_out();
    for(t=7; (t>=0)&&(t<=7); t--)
    { 	SCL(0);            
        if((Byte>>t)&0x01)
        SDA(1);
        else
        SDA(0);
		delayus(2);	
	   	SCL(1);
		delayus(2);	
	   	SCL(0);
		delayus(2);	
    }	
 

} 




//Read one byte£¬ack=0£¬Send Acknowledge£¬ack=1£¬NO Acknowledge   
unsigned char TOUCH_Read_Byte(unsigned char ack)
{	unsigned char t,receive=0;
	SDA_out();
	SCL(0);
	SDA(1);
	_nop_();
	SDA_input();	
	for(t = 0; t < 8; t++)
	{	_nop_();
	 	SCL(1);
		_nop_();
	 	receive<<=1;
	 	if(SDA_read)
	 	receive=receive|0x01;
		delayus(2);
	 	SCL(0);
		delayus(2);
	}

					 
   	if (ack)  TOUCH_NAck();//NO Acknowledge 
   	else       TOUCH_Ack(); //Send Acknowledge   
    
	 return receive;
}



//GSLX680_I2C_Write
static void GSLX680_I2C_Write(unsigned char *reg, unsigned char *addr, unsigned long cnt)
{
	unsigned int i=0;

	
	TOUCH_Start();
	TOUCH_Send_Byte(WRITE_ADD);
	TOUCH_Wait_Ack();	
	TOUCH_Send_Byte(*reg);
	TOUCH_Wait_Ack();

	for(i=0;i<cnt;i++,addr++)//data
	{		
		TOUCH_Send_Byte(*addr);
		TOUCH_Wait_Ack();
	}
	TOUCH_Stop();
}

//GSLX680_I2C_Read
static void GSLX680_I2C_Read(unsigned char *reg, unsigned char *addr, unsigned long cnt)
{
       unsigned int num=0;

	TOUCH_Start();
	TOUCH_Send_Byte(WRITE_ADD);
	TOUCH_Wait_Ack();	
	TOUCH_Send_Byte(*reg);
	TOUCH_Wait_Ack();
	TOUCH_Stop();

	TOUCH_Start();	
	TOUCH_Send_Byte(READ_ADD);//chip adress
	TOUCH_Wait_Ack();	
	
	for(num=0;num<cnt;num++)
  	{
		if(num==(cnt-1))  
		addr[num]=TOUCH_Read_Byte(1);   
        else 
    	addr[num]=TOUCH_Read_Byte(0);   
  	}	
	
	TOUCH_Stop();


}


//get the most data about capacitive touchpanel.
unsigned char GSLX680_read_data(void)
{	unsigned char touch_data[24] = {0}; 
	unsigned char reg = 0x80;
	GSLX680_I2C_Read(&reg, touch_data, 24);
	
	ts_event.y5 = (unsigned int)(touch_data[23])<<8 | (unsigned int)touch_data[22];
	ts_event.x5 = (unsigned int)(touch_data[21])<<8 | (unsigned int)touch_data[20];


	ts_event.y4 = (unsigned int)(touch_data[19])<<8 | (unsigned int)touch_data[18];
	ts_event.x4 = (unsigned int)(touch_data[17])<<8 | (unsigned int)touch_data[16];


	ts_event.y3 = (unsigned int)(touch_data[15])<<8 | (unsigned int)touch_data[14];
	ts_event.x3 = (unsigned int)(touch_data[13])<<8 | (unsigned int)touch_data[12];


	ts_event.y2 = (unsigned int)(touch_data[11])<<8 | (unsigned int)touch_data[10];
	ts_event.x2 = (unsigned int)(touch_data[9])<<8 | (unsigned int)touch_data[8];


	ts_event.y1 = ((unsigned int)(touch_data[7])<<8 | (unsigned int)touch_data[6]) &0xFFF;
	ts_event.x1 = ((unsigned int)(touch_data[5])<<8 | (unsigned int)touch_data[4]) &0xFFF;
	ts_event.touch_point = touch_data[0];
	if(touch_data[0])
	{
		ts_event.touch_hold_count =0;
		ts_event.touch_event = TOUCH_DOWN;
	}

	return touch_data[0];	
}




void inttostr(unsigned int value,unsigned char *str)
{
	str[0]=value/1000+0x30;
	str[1]=value%1000/100+0x30;
	str[2]=value%1000%100/10+0x30;
	str[3]=value%1000%100%10+0x30;

}




//TP interrrupt    GSL1680 is high levels interrrupt   MCU is low levels interrrupt     So here need to connect an inverter
////////////////////////////////////
//void  counter0(void) interrupt 0
//{
// 	if(PEN==0)										//Detect the occurrence of an interrupt
// 	{
//		ts_event.Key_Sta=Key_Down;                              
//		led=~led;
// 	}
//}


//GSLX680 Clear reg
static void _GSLX680_clr_reg(void)
{
	unsigned char addr;	
	unsigned char Wrbuf[4];

	addr = 0xe0;
	Wrbuf[0] = 0x88;
	GSLX680_I2C_Write(&addr, Wrbuf, 1); 	
	delayus(200);
	addr = 0x80;
	Wrbuf[0] = 0x01;
	GSLX680_I2C_Write(&addr, Wrbuf, 1); 	
	delayus(50);
	addr = 0xe4;
	Wrbuf[0] = 0x04;
	GSLX680_I2C_Write(&addr, Wrbuf, 1); 	
	delayus(50);
	addr = 0xe0;
	Wrbuf[0] = 0x00;
	GSLX680_I2C_Write(&addr, Wrbuf, 1); 	
	delayus(50);
}
//_GSLX680 Reset
static void _GSLX680_reset_chip(void)
{
	unsigned char addr;	
	unsigned char Wrbuf[4];
	

	addr = 0xe0;
	Wrbuf[0] = 0x88;
       GSLX680_I2C_Write(&addr,Wrbuf, 1);
	delayus(50);

	addr = 0xe4;
	Wrbuf[0] = 0x04;
       GSLX680_I2C_Write(&addr,Wrbuf, 1);
	delayus(50);

	addr = 0xbc;
	Wrbuf[0] = 0x00;
	Wrbuf[1] = 0x00;
	Wrbuf[2] = 0x00;
	Wrbuf[3] = 0x00;
       GSLX680_I2C_Write(&addr,Wrbuf, 4);
	delayus(50);
	   
}
 
//GSLX680 Main Down
static void _GSLX680_load_fw(void)
{
	unsigned char addr;
	unsigned char Wrbuf[4];
	unsigned int source_line=0;
	unsigned int source_len = sizeof(GSLX680_FW)/sizeof(struct fw_data);
	

	for (source_line=0; source_line<source_len; source_line++)
	{
		addr = GSLX680_FW[source_line].offset;
		Wrbuf[0] = (char)(GSLX680_FW[source_line].val & 0x000000ff);
		Wrbuf[1] = (char)((GSLX680_FW[source_line].val & 0x0000ff00) >> 8);
		Wrbuf[2] = (char)((GSLX680_FW[source_line].val & 0x00ff0000) >> 16);
		Wrbuf[3] = (char)((GSLX680_FW[source_line].val & 0xff000000) >> 24);

		GSLX680_I2C_Write(&addr,Wrbuf, 4);
	}
	

}
//startup chip
static void _GSLX680_startup_chip(void)
{
	unsigned char addr;	
	unsigned char Wrbuf[4];

	addr = 0xe0;
	Wrbuf[0] = 0x00;
  GSLX680_I2C_Write(&addr,Wrbuf, 1);

}

//GSLX680_Initial
void TOUCH_Init(void)
{
	SCL_out();	
	WAKE(1);	
	HAL_Delay(20);
	WAKE(0);		
	HAL_Delay(20);
	WAKE(1);		
	HAL_Delay(20);
	DEBUG_INFO("touch_ena\r\n");
	_GSLX680_clr_reg();
	_GSLX680_reset_chip();
	_GSLX680_load_fw();
//	_GSLX680_startup_chip();
	_GSLX680_reset_chip();
	_GSLX680_startup_chip();
}


/******************************************************************************************
*Function name£ºDraw_Big_Point(u16 x,u16 y)
* Parameter£ºuint16_t x,uint16_t y xy
* Return Value£ºvoid
* Function£ºDraw touch pen nib point 2 * 2
*********************************************************************************************/		   
//void Draw_Big_Point(unsigned int x,unsigned int y,unsigned int colour)
//{ unsigned char tmp1,tmp2;
//	tmp1=colour>>8;tmp2=colour;
//   
//    Write_Dir(0x40,0x00);//The drawing mode	
//	MemoryWrite_Position(x,y);//Memory write position
//	LCD_CmdWrite(0x02);//Memory write Data	
//	LCD_DataWrite(tmp1);
//	LCD_DataWrite(tmp2);
//	LCD_DataWrite(tmp1);
//	LCD_DataWrite(tmp2);
//	MemoryWrite_Position(x,y+1);//Memory write position
//    LCD_CmdWrite(0x02);////Memory write Data					   
//	LCD_DataWrite(tmp1);
//	LCD_DataWrite(tmp2);
//	LCD_DataWrite(tmp1);
//	LCD_DataWrite(tmp2);
//}
