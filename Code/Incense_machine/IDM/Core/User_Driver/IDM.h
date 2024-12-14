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


typedef enum
{
		STARTUP				=0x00,
		INSENCE_EMPTY,
		HELLO_CUSTOM,	
    CELL_SUCCESS,
		SETTING_1,
		SETTING_2,
		DONATE,	
		WAIT_INSENCE,
		STOP_SERVICE,
		SALES_SERVICE,
} idm_state_cell_t;

typedef struct {
	 bool enable;
	 unsigned int timeOn;	//ms
	 unsigned int timeCycle;
	 unsigned int timeActive;		//ms
	 unsigned int counter;
} LED;

typedef struct {
	 bool isRun;
	 unsigned int Current;
	 unsigned int MaxCurrent;
	 bool isOverCurrent;
	 bool stripOverShutdown;			//activity when over
} IDM_OUT;

typedef struct {
	 bool isMotorERROR;
	 bool isPaymentERROR;
	 bool isDropERROR;
} IDM_ERROR;


typedef struct {
	IDM_OUT Motor_conveyer;
	IDM_OUT Motor_swap;
	IDM_OUT Solenoid;
	bool isHeater;
	bool isDoorOpen;
	bool isIsenseDrop;
	bool isEmptyIsenseSW;
	char LedState;
} IDM_HARDWARE;

typedef struct {
	unsigned short int header;		// id off para	
	unsigned char NumberInsenseBuy;		// so que moi lan ban 1-100	(N_2)
	unsigned char TimeSWAPRun;	// thoi gian swap quay 0-100s (N_1)
	unsigned short int TotalInsenseCycleSwapBuy;					// so luong que da ban can quay swap motor 1-1000		(N_3)
	unsigned short int NumberBuyMore;		// so luong ban khi bao Empty 1-1000	(N_4)
	unsigned char TimeDependIsense;		// bo qua
	unsigned char TimeTimeout;		// so lan retry k du huong
	unsigned char EnableHumidity;
	unsigned char HumidityMAX;	// gia tr? cai dat do am
	unsigned short int currentNumberBuyMore;		// so luong ban khi bao Empty 1-1000
	unsigned short int currentTotalInsenseBuy;					// swap motor in when total =0;
	unsigned char retryCellEmpty;		// so lan ban khi huong khong ra
	unsigned char currentRetryCellEmpty;
} __attribute__((packed)) IDM_PARA;

typedef struct {
	unsigned int Money;
	unsigned int TotalIsenseDroped;
	unsigned char numberRetry;
	idm_state_cell_t	StateBuy;
} BUY_PARA;

#define MAXTimeDropRespone	150		//ms

/* Private function prototypes -----------------------------------------------*/
void IDM_init(void);
void Scan_IDM(void);
void check_stopService(void);
unsigned char set_mode(unsigned char mode);

/* Private variables ---------------------------------------------------------*/

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
