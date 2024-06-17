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
unsigned char HumidityIDM = 30;

/* Private define ------------------------------------------------------------*/
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */  
void IDM_init(void)
{
//default	
	IDM.Humidity = 30;
	IDM.NumberBuyEmpty = 10;
	IDM.NumberInsenseBuy = 8;
	IDM.TimeConveyerRun = 40;
	IDM.TotalInsenseBuy = 1000;
}
void Scan_IDM(void)
{
	if(IDM.Humidity >= HumidityIDM)
		IDM_Status.isHeater = true;
	else
		IDM_Status.isHeater = false;
	
	if(IDM_Status.isHeater)	HAL_GPIO_WritePin(Heater_GPIO_Port, Heater_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(Heater_GPIO_Port, Heater_Pin, GPIO_PIN_RESET);
	
	if(IDM_Status.Motor_conveyer.isRun)	HAL_GPIO_WritePin(CONVEYER_MOTOR_GPIO_Port, CONVEYER_MOTOR_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(CONVEYER_MOTOR_GPIO_Port, CONVEYER_MOTOR_Pin, GPIO_PIN_RESET);
	
	if(IDM_Status.Motor_swap.isRun)	HAL_GPIO_WritePin(SWAP_MOTOR_GPIO_Port, SWAP_MOTOR_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(SWAP_MOTOR_GPIO_Port, SWAP_MOTOR_Pin, GPIO_PIN_RESET);
	
	if(IDM_Status.Solenoid.isRun)	HAL_GPIO_WritePin(SOLENOID_GPIO_Port, SOLENOID_Pin, GPIO_PIN_SET);
	else	HAL_GPIO_WritePin(SOLENOID_GPIO_Port, SOLENOID_Pin, GPIO_PIN_RESET);
	
}

/* Private function prototypes -----------------------------------------------*/


