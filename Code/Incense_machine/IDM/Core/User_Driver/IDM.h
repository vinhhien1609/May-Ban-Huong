/*********************************************************************************************************
*
* File                : RTC_Time.h
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#ifndef _IDM_H_
#define _IDM_H_

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
	 bool isRun;
	 unsigned int Current;
	 unsigned int MaxCurrent;
	 bool isOverCurrent;
	 bool stripOverShutdown;			//activity when over
} IDM_OUT;

typedef struct {
	IDM_OUT Motor_conveyer;
	IDM_OUT Motor_swap;
	IDM_OUT Solenoid;
	bool isHeater;
	bool isDoorOpen;
	bool isIsenseDrop;
	bool isEmptyIsense;
	char LedState;
} IDM_STT;

typedef struct {
	unsigned char NumberInsenseBuy;		// so que moi lan ban 1-100
	unsigned char TimeConveyerRun;	// thoi gian conveyer quay 1-100s
	unsigned int TotalInsenseBuy;					// tong so que cos the ban 1-1000
	unsigned int NumberBuyEmpty;		// so luong ban khi bao Empty 1-1000
	unsigned char Humidity;		// so luong ban khi bao Empty 1-100
} IDM_PARA;

/* Private function prototypes -----------------------------------------------*/
void IDM_Init(void);

/* Private variables ---------------------------------------------------------*/

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/