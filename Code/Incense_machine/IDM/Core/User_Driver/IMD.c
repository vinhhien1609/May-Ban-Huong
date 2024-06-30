/*********************************************************************************************************
*
* File                : RTC_Time.c
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

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"
#include "stm32f1xx_hal.h"
//#include "stm32f1xx_hal_rtc.c"
#include "IDM.h"
#include "main.h"
IDM_PARA IDM;
IDM_STT IDM_Status;
BUY_PARA buy;
unsigned char havemoney=0, IDM_state=0, State=0xFF;

/* Private define ------------------------------------------------------------*/
uint32_t count=0;
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */  
void IDM_init(void)
{
//default	
	IDM.Humidity = 0;
	IDM.HumidityMAX = 90;
	IDM.NumberBuyEmpty = 10;
	IDM.NumberInsenseBuy = 8;
	IDM.TimeConveyerRun = 30;
	IDM.TimePUSHIsense = 2;
	IDM.TotalInsenseBuy = 1000;
}
void Scan_IDM(void)
{

	switch (IDM_state)
	{
		case 0:		//start Machine
			if(State!=IDM_state)
			{
				count=0;
				State = IDM_state;
				IDM_Status.Motor_swap.isRun =1;				
			}
			else
			{
				if(count/1000 >=IDM.TimePUSHIsense)	// dao huong
				{
					IDM_Status.Motor_swap.isRun =0;
					IDM_state =1;
				}
			}
			break;
		case 1:		// wait guide
			if(State!=IDM_state)
			{
				count=0;
				State = IDM_state;
				IDM_Status.Motor_conveyer.isRun =0;
				IDM_Status.Motor_swap.isRun = 0;
			}
			else
			{
				if(havemoney)
				{
					havemoney =0;
					IDM_state = 2;
				}
			}
			break;
		case 2:		// Buying
			if(State!=IDM_state)
			{
				count=MAXTimeDropRespone-10;
				State = IDM_state;
				buy.TotalIsenseDroped = IDM.NumberInsenseBuy;
				IDM_Status.Motor_conveyer.isRun =1;
			}
			else
			{
				if(count/1000 >=IDM.TimeConveyerRun)
				{
					IDM_state =3;
				}
				if(buy.TotalIsenseDroped==0)
				{
					IDM_state =4;
				}
			}				
			break;
		case 3:		// qua thoi gian ma khong thay huong--> Dao huong
			if(State!=IDM_state)
			{
				count=0;
				IDM_Status.Motor_swap.isRun =1;		// dao huong
				State = IDM_state;
			}
			else
			{
				if(count/1000>=IDM.TimePUSHIsense)
				{
					IDM_Status.Motor_swap.isRun =0;
				}
				if(count/1000 >=IDM.TimeConveyerRun)
				{
					IDM_state =1;
				}
				if(buy.TotalIsenseDroped==0)
				{
					IDM_state =4;
				}
			}
			break;
		case 4:
			if(State!=IDM_state)
			{
				count=0;
				IDM_Status.Motor_conveyer.isRun =0;
				IDM_Status.Solenoid.isRun=1;
				State = IDM_state;
			}
			else
			{
				if(count/1000 >=1)
				{
					IDM_Status.Solenoid.isRun=0;
					IDM_state =1;
				}
			}
			break;
	}

	
	if(IDM.Humidity >= IDM.HumidityMAX)
		IDM_Status.isHeater = true;
	else
		IDM_Status.isHeater = false;
	
	if(IDM_Status.isHeater==true)	HAL_GPIO_WritePin(Heater_GPIO_Port, Heater_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(Heater_GPIO_Port, Heater_Pin, GPIO_PIN_RESET);
	
	if(IDM_Status.Motor_conveyer.isRun)	HAL_GPIO_WritePin(CONVEYER_MOTOR_GPIO_Port, CONVEYER_MOTOR_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(CONVEYER_MOTOR_GPIO_Port, CONVEYER_MOTOR_Pin, GPIO_PIN_RESET);
	
	if(IDM_Status.Motor_swap.isRun)	HAL_GPIO_WritePin(SWAP_MOTOR_GPIO_Port, SWAP_MOTOR_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(SWAP_MOTOR_GPIO_Port, SWAP_MOTOR_Pin, GPIO_PIN_RESET);
	
	if(IDM_Status.Solenoid.isRun)	HAL_GPIO_WritePin(SOLENOID_GPIO_Port, SOLENOID_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(SOLENOID_GPIO_Port, SOLENOID_Pin, GPIO_PIN_RESET);
	
}

/* Private function prototypes -----------------------------------------------*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == Drop_Pin)
	{
		if(count>= MAXTimeDropRespone)
		{
			if(buy.TotalIsenseDroped)	buy.TotalIsenseDroped--;
			if(buy.TotalIsenseDroped==0)	IDM_Status.Motor_conveyer.isRun=0;
			count =0;		// chong rung			
		}

	}
}

