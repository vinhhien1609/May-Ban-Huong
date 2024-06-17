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
#include "RTC_Time.h"

/* Private define ------------------------------------------------------------*/
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */  

TimeStruct currentTime;
extern RTC_HandleTypeDef hrtc;

/* Private function prototypes -----------------------------------------------*/
void Time_Set(uint32_t t);
static uint8_t RTC_WeekDayNum(uint32_t nYear, uint8_t nMonth, uint8_t nDay);

/**
  * @brief  Enters the RTC Initialization mode.
  * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval HAL status
  */
static HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  tickstart = HAL_GetTick();
  /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);


  return HAL_OK;
}

/**
  * @brief  Exit the RTC Initialization mode.
  * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval HAL status
  */
static HAL_StatusTypeDef RTC_ExitInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  tickstart = HAL_GetTick();
  /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

static uint32_t RTC_ReadCounter(RTC_HandleTypeDef *hrtc)
{
  uint16_t high1 = 0U, high2 = 0U, low = 0U;
  uint32_t timecounter = 0U;

  high1 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);
  low   = READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT);
  high2 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);

  if (high1 != high2)
  {
    /* In this case the counter roll over during reading of CNTL and CNTH registers,
       read again CNTL register then return the counter value */
    timecounter = (((uint32_t) high2 << 16U) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  {
    /* No counter roll over during reading of CNTL and CNTH registers, counter
       value is equal to first value of CNTL and CNTH */
    timecounter = (((uint32_t) high1 << 16U) | low);
  }

  return timecounter;
}
/**
  * @brief  Write the time counter in RTC_CNT registers.
  * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @param  TimeCounter: Counter to write in RTC_CNT registers
  * @retval HAL status
  */
static HAL_StatusTypeDef RTC_WriteCounter(RTC_HandleTypeDef *hrtc, uint32_t TimeCounter)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Set Initialization mode */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Set RTC COUNTER MSB word */
    WRITE_REG(hrtc->Instance->CNTH, (TimeCounter >> 16U));
    /* Set RTC COUNTER LSB word */
    WRITE_REG(hrtc->Instance->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));

    /* Wait for synchro */
    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  return status;
}
/*******************************************************************************
* Function Name  : Time_ConvUnixToCalendar
* Description    : 
* Input          : 
* Output         : None
* Return         : struct tm
* Attention		 : None
*******************************************************************************/
struct tm Time_ConvUnixToCalendar(time_t t)
{
	struct tm *t_tm;
	t_tm = localtime(&t);
	t_tm->tm_year += 1900;
	return *t_tm;
}

/*******************************************************************************
* Function Name  : Time_ConvCalendarToUnix
* Description    : 
* Input          : - t: struct tm
* Output         : None
* Return         : time_t
* Attention		 : None
*******************************************************************************/
time_t Time_ConvCalendarToUnix(struct tm t)
{
	t.tm_year -= 1900;
	return mktime(&t);
}


/*******************************************************************************
* Function Name  : Time_GetUnixTime
* Description    : 
* Input          : None
* Output         : None
* Return         : time_t
* Attention		 : None
*******************************************************************************/
time_t Time_GetUnixTime(void)
{
	return (time_t)RTC_ReadCounter(&hrtc);
}

/*******************************************************************************
* Function Name  : Time_GetCalendarTime
* Description    : 
* Input          : None
* Output         : None
* Return         : struct tm
* Attention		 : None
*******************************************************************************/
struct tm Time_GetCalendarTime(void)
{
	time_t t_t;
	struct tm t_tm;

	t_t = (time_t)RTC_ReadCounter(&hrtc);
	t_tm = Time_ConvUnixToCalendar(t_t);
	return t_tm;
}

/*******************************************************************************
* Function Name  : Time_SetUnixTime
* Description    : 
* Input          : - t: time_t 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Time_SetUnixTime(time_t t)
{
//	RTC_WaitForLastTask();
	RTC_WriteCounter(&hrtc,(uint32_t)t);
//	RTC_WaitForLastTask();
	return;
}

/*******************************************************************************
* Function Name  : Time_SetCalendarTime
* Description    : 
* Input          : - t: struct tm
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Time_SetCalendarTime(struct tm t)
{
	Time_SetUnixTime(Time_ConvCalendarToUnix(t));
	return;
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void NVIC_Configuration(void)
{
//  NVIC_InitTypeDef NVIC_InitStructure;

//  /* Configure one bit for preemption priority */
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

//  /* Enable the RTC Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void RTC_Configuration(void)
{
//  /* Enable PWR and BKP clocks */
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

//  /* Allow access to BKP Domain */
//  PWR_BackupAccessCmd(ENABLE);

//  /* Reset Backup Domain */
//  BKP_DeInit();

//  /* Enable LSE */
//  RCC_LSEConfig(RCC_LSE_ON);
//  /* Wait till LSE is ready */
//  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
//  {}

//  /* Select LSE as RTC Clock Source */
//  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

//  /* Enable RTC Clock */
//  RCC_RTCCLKCmd(ENABLE);

//  /* Wait for RTC registers synchronization */
//  RTC_WaitForSynchro();

//  /* Wait until last write operation on RTC registers has finished */
//  RTC_WaitForLastTask();

//  /* Enable the RTC Second */
//  RTC_ITConfig(RTC_IT_SEC, ENABLE);

//  /* Wait until last write operation on RTC registers has finished */
//  RTC_WaitForLastTask();

//  /* Set RTC prescaler: set RTC period to 1sec */
//  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

//  /* Wait until last write operation on RTC registers has finished */
//  RTC_WaitForLastTask();
//	BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
}

// /*******************************************************************************
// * Function Name  : USART_Scanf
// * Description    : USART Receive
// * Input          : - min_value: 
// *                  - max_value:
// *                  - lenght:
// * Output         : None
// * Return         : uint8_t
// * Attention		 : None
// *******************************************************************************/
// static uint16_t USART_Scanf(uint32_t min_value,uint32_t max_value,uint8_t lenght)
// {
//   uint16_t index = 0;
//   uint32_t tmp[4] = {0, 0, 0, 0};

//   while (index < lenght)
//   {
//     /* Loop until RXNE = 1 */
//     while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
//     {}
//     tmp[index++] = (USART_ReceiveData(USART1));

// 	if( tmp[index - 1] == 0x0D ) { index--; continue; }

//     if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
//     {
// //      printf("Please enter valid number between 0 and 9\r\n");
//       index--;
//     }
//   }
//   /* Calculate the Corresponding value */
//   if( lenght ==2 )
//   index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10 );

//   else  /* lenght ==4 */
//   index = (tmp[3] - 0x30) + ((tmp[2] - 0x30) * 10 ) + ((tmp[1] - 0x30) * 100 ) + ((tmp[0] - 0x30) * 1000 );
//   /* Checks */
//   if (index > max_value || index < min_value)
//   {
// //	printf("Please enter valid number between %d and %d\r\n", min_value, max_value);
//     return 0;
//   }


//   return index;
// }

/*******************************************************************************
* Function Name  : Time_Regulate
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void setRTC(TimeStruct ttime)
{
  struct tm time;

  memset(&time, 0 , sizeof(time) );
	RTC_Configuration();
//  printf("=======================Time Settings==========================\r\n");
  time.tm_year=ttime.year + 2000;
  time.tm_mon=ttime.month-1;
  time.tm_mday=ttime.date;
  time.tm_hour=ttime.hour;
  time.tm_min=ttime.minute;
  time.tm_sec=ttime.second;
  /* Return the value to store in RTC counter register */
  Time_SetCalendarTime(time);  
}

/*******************************************************************************
* Function Name  : RTC_Init
* Description    : RTC Initialization
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None	
*******************************************************************************/
void RTC_Init(void)
{
	TimeStruct t;
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0xA5A5)
  {
    /* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */

 //   printf("RTC not yet configured....\r\n");

    /* RTC Configuration */
    RTC_Configuration();
		t.year = 18;
		t.month = 9;
		t.date = 16;
		t.hour=9;
		t.minute=0;
		t.second=0;		
		setRTC(t);

	/* Adjust time by values entred by the user on the hyperterminal */

  //  printf("RTC configured....\r\n");

    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0xA5A5);
  }
//  else
//  {
//    /* Check if the Power On Reset flag is set */
//    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
//    {
//  //    printf("Power On Reset occurred....\r\n");
//    }
//    /* Check if the Pin Reset flag is set */
//    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
//    {
////      printf("External Reset occurred....\r\n");
//    }
//// 		RTC_Configuration();
//// 		Time_Regulate();
// //   printf("No need to configure RTC....\r\n");
//    /* Wait for RTC registers synchronization */
//    RTC_WaitForSynchro();

//    /* Enable the RTC Second */
//    RTC_ITConfig(RTC_IT_SEC, ENABLE);
//    /* Wait until last write operation on RTC registers has finished */
//    RTC_WaitForLastTask();
//  }

   /* NVIC configuration */
   NVIC_Configuration();

#ifdef RTCClockOutput_Enable
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Disable the Tamper Pin */
  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
                                 functionality must be disabled */

  /* Enable RTC Clock Output on Tamper Pin */
  BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

   /* Clear reset flags */
//  RCC_ClearFlag();
	
  return;
}
/**
  * @brief  Determines the week number, the day number and the week day number.
  * @param  nYear   year to check
  * @param  nMonth  Month to check
  * @param  nDay    Day to check
  * @note   Day is calculated with hypothesis that year > 2000
  * @retval Value which can take one of the following parameters:
  *         @arg RTC_WEEKDAY_MONDAY
  *         @arg RTC_WEEKDAY_TUESDAY
  *         @arg RTC_WEEKDAY_WEDNESDAY
  *         @arg RTC_WEEKDAY_THURSDAY
  *         @arg RTC_WEEKDAY_FRIDAY
  *         @arg RTC_WEEKDAY_SATURDAY
  *         @arg RTC_WEEKDAY_SUNDAY
  */
static uint8_t RTC_WeekDayNum(uint32_t nYear, uint8_t nMonth, uint8_t nDay)
{
  uint32_t year = 0U, weekday = 0U;

  year = 2000U + nYear;

  if (nMonth < 3U)
  {
    /*D = { [(23 x month)/9] + day + 4 + year + [(year-1)/4] - [(year-1)/100] + [(year-1)/400] } mod 7*/
    weekday = (((23U * nMonth) / 9U) + nDay + 4U + year + ((year - 1U) / 4U) - ((year - 1U) / 100U) + ((year - 1U) / 400U)) % 7U;
  }
  else
  {
    /*D = { [(23 x month)/9] + day + 4 + year + [year/4] - [year/100] + [year/400] - 2 } mod 7*/
    weekday = (((23U * nMonth) / 9U) + nDay + 4U + year + (year / 4U) - (year / 100U) + (year / 400U) - 2U) % 7U;
  }

  return (uint8_t)weekday;
}
/*******************************************************************************
* Function Name  : Time_Display
* Description    : Printf Time
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
TimeStruct getRTC(void)
{
   struct tm time;
	TimeStruct timeRead;
   time = Time_GetCalendarTime();
	 timeRead.second = time.tm_sec;
	 timeRead.minute = time.tm_min;
	 timeRead.hour = time.tm_hour;
	 timeRead.date = time.tm_mday;
	 timeRead.month = time.tm_mon+1;
	 timeRead.year = time.tm_year%100;
	 timeRead.dayofDate = (char)RTC_WeekDayNum(timeRead.year, timeRead.month, timeRead.date);
	 return (timeRead);
}

