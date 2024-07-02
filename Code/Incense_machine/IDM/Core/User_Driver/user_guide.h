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
unsigned char scan_BT(void);
void Menu_draw(void);

/* Private variables ---------------------------------------------------------*/
typedef enum displayType{
	Home = 0U,
	Password,
	Setup,
	TimeSetup
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
