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

#ifndef _USER_GUIDE_H_
#define _USER_GUIDE_H_

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x.h"
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>



/* Private function prototypes -----------------------------------------------*/
void Menu_draw(void);
void Menu_draw_MONO(void);
void Menu_draw_TFT(void);


/* Private variables ---------------------------------------------------------*/
typedef enum displayType{
	DIS_Home = 0U,
	DIS_Password,
	DIS_Setup,
	DIS_ACCEPT_DENO,
	DIS_TimeSetup,
	DIS_TOTAL_MONEY,
		DIS_DAY,
		DIS_MONTH,
		DIS_YEAR,
		DIS_TOTAL,
	DIS_CELL_EACH_TIME,
		
	DIS_ERROR_INFO,
		DIS_ERROR_MOTOR,
		DIS_ERROR_PAYMENT,
		DIS_ERROR_SENSOR,
		DIS_DEL_ERROR_MOTOR,
		DIS_DEL_ERROR_HW,
		DIS_DEL_ERROR_SENSOR,
	DIS_ONOFF_HUMIDITY,
	DIS_HUMIDITY_SET,
	
	DIS_TIME_SWAP_RUN,
	DIS_NUMBER_CELL_SWAP,
	DIS_CELL_MORE,
	DIS_NUMBER_CELL_MORE_EMPTY,
	DIS_DEL_CELL_ERROR,
	DIS_SHOW_MESSENGER,
	TOTAL_DIS,
	
	DIS_SETUP_TECH,
		DIS_SET_ID_MACHINE,
		DIS_SET_IP,
	DIS_KEYBOARD,
	DIS_PRICE_SET,
} Display_Typedef;
#endif


//typedef enum
//{
//		VDM_LANG_XIN_KINH_CHAO,
//		VDM_QUY_KHACH,
//    VDM_LANG_ID_MAX
//} vdm_language_id_t;




/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
