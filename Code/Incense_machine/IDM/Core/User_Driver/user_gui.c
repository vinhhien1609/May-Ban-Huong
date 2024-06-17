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
#include "USER_GUIDE.h"
#include "main.h"
#define KEY_HOLE_TIME 20
unsigned char key_map[16]= {'0','1','2','3','4','5','6','7','8','9','*','#'};
unsigned char key_count=0, old_key=255, pressed_old_key=255;
/* Private define ------------------------------------------------------------*/
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */  


static void small_delay()		// not importance function
{
	volatile uint32_t i = 0x07F;
	while (i--);
}

unsigned char scan_BT(void)
{
	unsigned char key=255;
	HAL_GPIO_WritePin(KEY_COL1_GPIO_Port, KEY_COL1_Pin, GPIO_PIN_SET);
	small_delay();
	if(HAL_GPIO_ReadPin(KEY_ROW1_GPIO_Port, KEY_ROW1_Pin))	key =1;
	if(HAL_GPIO_ReadPin(KEY_ROW2_GPIO_Port, KEY_ROW2_Pin))	key =4;
	if(HAL_GPIO_ReadPin(KEY_ROW3_GPIO_Port, KEY_ROW3_Pin))	key =7;
	if(HAL_GPIO_ReadPin(KEY_ROW4_GPIO_Port, KEY_ROW4_Pin))	key =10;	//*
	HAL_GPIO_WritePin(KEY_COL1_GPIO_Port, KEY_COL1_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(KEY_COL2_GPIO_Port, KEY_COL2_Pin, GPIO_PIN_SET);
	small_delay();
	if(HAL_GPIO_ReadPin(KEY_ROW1_GPIO_Port, KEY_ROW1_Pin))	key =2;
	if(HAL_GPIO_ReadPin(KEY_ROW2_GPIO_Port, KEY_ROW2_Pin))	key =5;
	if(HAL_GPIO_ReadPin(KEY_ROW3_GPIO_Port, KEY_ROW3_Pin))	key =8;
	if(HAL_GPIO_ReadPin(KEY_ROW4_GPIO_Port, KEY_ROW4_Pin))	key =0;
	HAL_GPIO_WritePin(KEY_COL2_GPIO_Port, KEY_COL2_Pin, GPIO_PIN_RESET);	
	
	HAL_GPIO_WritePin(KEY_COL3_GPIO_Port, KEY_COL3_Pin, GPIO_PIN_SET);
	small_delay();
	if(HAL_GPIO_ReadPin(KEY_ROW1_GPIO_Port, KEY_ROW1_Pin))	key =3;
	if(HAL_GPIO_ReadPin(KEY_ROW2_GPIO_Port, KEY_ROW2_Pin))	key =6;
	if(HAL_GPIO_ReadPin(KEY_ROW3_GPIO_Port, KEY_ROW3_Pin))	key =9;
	if(HAL_GPIO_ReadPin(KEY_ROW4_GPIO_Port, KEY_ROW4_Pin))	key =11;	//#
	HAL_GPIO_WritePin(KEY_COL3_GPIO_Port, KEY_COL3_Pin, GPIO_PIN_RESET);

	if(old_key==key && key <255)
	{
		if(key_count<KEY_HOLE_TIME)
			key_count ++;	
		else
		{
			if(key !=pressed_old_key)
			{
				pressed_old_key = key;
				HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
				HAL_Delay(10);
				HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
//				return key_map[key];// chi gui 1 lan cho toi khi nha
			}
			return key_map[key];		//
		}
	}
	else
	{
		pressed_old_key =255;
		key_count =0;
		old_key = key;
	}
	return 255;
}
/* Private function prototypes -----------------------------------------------*/


