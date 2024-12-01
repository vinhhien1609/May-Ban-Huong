#ifndef TOUCH_H
#define TOUCH_H

#include "main.h"
//Touch Status	 
#define Key_Down 0x01
#define Key_Up   0x00 

#define SCREEN_MAX_X 		800
#define SCREEN_MAX_Y 		480

typedef enum
{
		NO_TOUCH				=0x00,
		TOUCH_DOWN,
		TOUCH_UP,	
    TOUCH_HOLD,
		START,
} touch_event_t;

struct _ts_event
{
    unsigned int    x1;
    unsigned int    y1;
    unsigned int    x2;
    unsigned int    y2;
    unsigned int    x3;
    unsigned int    y3;
    unsigned int    x4;
    unsigned int    y4;
    unsigned int    x5;
    unsigned int    y5;
    unsigned char   touch_point;
		unsigned char   touch_hold_count;
		touch_event_t   touch_event;
};

#define WRITE_ADD	0x80  
#define READ_ADD	0x81  

void WAKE(unsigned char x);
void TOUCH_Init(void);
void TOUCH_Start(void);
void TOUCH_Stop(void);
unsigned char   TOUCH_Wait_Ack(void);
void TOUCH_Ack(void);
void TOUCH_NAck(void);

void TOUCH_Send_Byte(unsigned char txd);
unsigned char TOUCH_Read_Byte(unsigned char ack);
void TOUCH_Wr_Reg(unsigned char RegIndex,unsigned char RegValue1);
void TOUCH_RdParFrPCTPFun(unsigned char *PCTP_Par,unsigned char ValFlag);
unsigned char   TOUCH_Read_Reg(unsigned char RegIndex);
void Draw_Big_Point(unsigned int x,unsigned int y,unsigned int colour);
unsigned char GSLX680_read_data(void);

#endif
