/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "RTC_Time.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <app_debug.h>

#include "nv11.h"
#include "ssp_defines.h"
#include "serialfunc.h"
#include "SSPComs.h"
#include "ssp_helpers.h"
#include "ITLSSPProc.h"
#include "GLCD.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint16_t tim1_cnt;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//static SSP_COMMAND m_ssp_cmd;
//static SSP6_SETUP_REQUEST_DATA m_setup_req;

uint8_t rx1_buffer[256];
uint8_t rx1_cnt, rx1_size;

uint32_t i;
uint8_t buff[3*256+2];
uint16_t time_led_delay_ms;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* RTC handler declaration */
RTC_HandleTypeDef hrtc;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
//void test_nv11(void)
//{
//	DEBUG_INFO("Init low-level done!\r\n");
//	HAL_Delay(1000);
//		DEBUG_INFO("Init NV11\r\n");
//		//-------------------------------------------------------------
//    ssp_serial_initialize();
//    ITLSSP_Init();
//		
//    ssp_serial_flush();
//    SSP_RESPONSE_ENUM nv11_response;

////    vdm_device_config_t *device_config = vdm_get_device_config();
//    memset(&m_ssp_cmd, 0, sizeof(m_ssp_cmd));
//    SSP_COMMAND *sspC = &m_ssp_cmd;
//		uint32_t i = 0;
//    sspC->PortNumber = 1;
//    sspC->SSPAddress = 0;
//    sspC->Timeout = 300;
//    sspC->EncryptionStatus = NO_ENCRYPTION;
//    sspC->RetryLevel = 3;
//    sspC->BaudRate = 9600;

////    for (i = 0; i < 10; i++)
////    {
////        m_refund_channels[i] = false; /* Xoa toan bo du lieu trong refundChannel */
////    }

//    /* Kiem tra su co mat cua NV11 (dau NV11 co duoc ket noi voi MCU khong) */
//    DEBUG_INFO("NV11 >> Getting information from Note Validator...\r\n");
//    /* 1. Ham kiem tra dong bo tra ket qua = 0xF0 -> Tim thay thiet bi */
//    nv11_response = ssp6_sync(sspC);
//    DEBUG_INFO("NV11 >> response: %u\r\n",nv11_response);		//hiendv
////    nv11_response = 0xF0;			//hiendv
//    if (nv11_response != SSP_RESPONSE_OK)
//    {
//      DEBUG_INFO("ERROR >> NV11 >> NO VALIDATOR FOUND\r\n");
//    }
//		else
//		{
//			DEBUG_INFO("NV11 >> Validator Found\r\n");
//		}
//		printf("\r\n"); HAL_Delay(1000);
//		
//    /* 2. Ham cai dat khoa ma hoa (Encryption key) mac dinh -> Ket qua phan hoi ve phai = 0xF0 (cai dat ok) */
//    nv11_response = ssp6_setup_encryption(sspC, (unsigned long long)0x123456701234567LL);
//    if (nv11_response != SSP_RESPONSE_OK)
//    {
//        DEBUG_INFO("ERROR >> NV11 >> Encryption Failed %d\r\n", nv11_response);
//    }
//    else
//    {
//        DEBUG_INFO("NV11 >> Encryption Setup\r\n");
//    }
//		printf("\r\n"); HAL_Delay(1000);
//		
//    /* 3. Ham kiem tra Version cua SSP co phai V6 -> Ham tra ve xac nhan la V6 = 0xF0 */
//    nv11_response = ssp6_host_protocol(sspC, 0x06);
//    if (nv11_response != SSP_RESPONSE_OK)
//    {
//        DEBUG_INFO("ERROR >> NV11 >> Host Protocol Failed %d\r\n", nv11_response);
//    }
//		else
//    {
//        DEBUG_INFO("NV11 >> Host Protocol Ver 6\r\n");
//    }
//		printf("\r\n"); HAL_Delay(1000);
//		
//    /* 4. Ham thu thap toan bo cac gia tri cai dat trong thiet bi (bao gom UnitType =0x07 (NV11), Firmwareversion (03760), Numberofchannel (6) , 
//       channeldata = Menh gia tien trong kenh, RealValueMultifier = 100, ProtocolVersion = 6*/
//    nv11_response = ssp6_setup_request(sspC, &m_setup_req);
//    if (nv11_response != SSP_RESPONSE_OK)
//    {
//        DEBUG_INFO("ERROR >> NV11 >> Setup Request Failed %d\r\n", nv11_response);
//    }
//		else
//    {
//			/* In ra man hinh debug Vision cua phan mem, va kenh tien te */
//			printf("Unit Type = %d\r\n",m_setup_req.UnitType);
//			DEBUG_INFO("NV11 >> Firmware: %s\r\n", m_setup_req.FirmwareVersion);
//			DEBUG_INFO("NV11 >> Channels:\r\n");
//			/* In ra man hinh menh gia tien cua tung kenh tien te: 1 = 1.000, 2 = 2.000, 3 = 5.000, 4 = 10.000, 5 = 20.000, 6 = 50.000*/
//			for (i = 0; i < m_setup_req.NumberOfChannels; i++)
//			{
//					DEBUG_INFO("NV11 >> channel %d: %d %s\r\n", i + 1, m_setup_req.ChannelData[i].value, m_setup_req.ChannelData[i].cc);
//			}
//    }
//		printf("\r\n"); HAL_Delay(1000);
//		
//		/* 6. */
//		nv11_response = ssp6_set_inhibits(sspC, 0x01, 0x00);
//		if (nv11_response != SSP_RESPONSE_OK)
//		{
//			DEBUG_INFO("ERROR >> NV11 >> Inhibits Failed\r\n");
//		}
//		else
//		{
//			DEBUG_INFO("NV11 >> Set Inhibits Done!\r\n");
//		}
//		printf("\r\n"); HAL_Delay(1000);

//		/* 4. Cho phep dau doc tien hoat dong -> Ket qua ok = 0xF0 */
//    nv11_response = ssp6_enable(sspC);
//    if (nv11_response != SSP_RESPONSE_OK)
//    {
//        DEBUG_INFO("ERROR >> NV11 >> Enable Failed %d\r\n", nv11_response);
//    }
//		else
//		{
//			DEBUG_INFO("NV11 >> Enable Successful\r\n");
//		}
//		printf("\r\n"); HAL_Delay(1000);
//		
//    /* 5. Neu UnitType = 0x03 -> Tuc la thiet bi dang ket noi la SMART Hopper*/
//    if (m_setup_req.UnitType == 0x03)
//    {
//			DEBUG_INFO("NV11 >>SMART Hopper = TRUE\r\n");
//        /* SMART Hopper requires different inhibit commands */
//        for (i = 0; i < m_setup_req.NumberOfChannels; i++)
//        {
//            /* Ham cai dat cac kenh tien te duoc phep tra lai */
//            ssp6_set_coinmech_inhibits(sspC, m_setup_req.ChannelData[i].value, m_setup_req.ChannelData[i].cc, ENABLED);
//        }
//    }
//    /* Neu la cac loai dau doc tien khac */
//    else
//    {
//			DEBUG_INFO("ERROR >> NV11 >>SMART Hopper = FALSE\r\n");
//		}
//		printf("\r\n"); HAL_Delay(1000);

//		/* 7. Lay so to tien chua trong FloatNote (dau tra lai) hien thi len man hinh */
//		int32_t stored_notes = ssp6_get_stored_notes(&m_ssp_cmd);
//		DEBUG_INFO("NV11 >> Stored Notes: %d\r\n", stored_notes);
//		printf("\r\n"); HAL_Delay(1000);
//		
//		/* 8. Cai dat menh gia tien te luu tru tren FloatNote de tra lai */
////		if (NV11_SetDenominationForChange(m_channel[channel_index], false))
////		{
////				return false;
////		}
////		m_refund_note = m_channel[channel_index]; /* Lay gia tri kenh tien te vao m_refund_note */
////		DEBUG_INFO("Refund note %u\r\n", m_refund_note);
//		
//		/* 0x06 -> dau doc SMART Payout, 0x07 -> dau doc NV11 cho phep de dua tien len tren */
//		if (m_setup_req.UnitType == 0x06 || m_setup_req.UnitType == 0x07)
//		{
//				/* Cho phep kich hoat tra lai tien -> ket qua ok = 0xF0 */
//				if (ssp6_enable_payout(sspC, m_setup_req.UnitType) != SSP_RESPONSE_OK)
//				{
//						DEBUG_INFO("ERROR >> NV11 >> Enable payout Failed\r\n");
////						m_count_init_payout_fail++;
////						DEBUG_INFO("NV11 >> Payout init %d times\r\n", m_count_init_payout_fail);

//						// if (m_count_init_payout_fail == 13)
//						// {
//						//     m_count_init_payout_fail = 0;
//						//     m_unit_type = m_setup_req.UnitType;
//						//     m_nv11_initialized = 1;
//						//     NV11_ClearLatestEvent(false);
//						//     NV11_DisplayOn();
//						//     m_init_payout_fault = true;
//						//     DEBUG_ERROR("Init payout fault\r\n");
//						//     return true;
//						// }
//				}
//				else
//				{
////						NV11_ClearLatestEvent(false);
////						NV11_DisplayOn();
//						DEBUG_INFO("NV11 >> Enable payout success\r\n");
//				}
//		}
//		printf("\r\n"); HAL_Delay(1000);
//}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

struct __FILE { int handle; };
FILE __stdout;
FILE __stdin;

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library DEBUG_INFO function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	char day[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
	char s[10];
	RTC_DateTypeDef Current_date;
	RTC_TimeTypeDef Current_time;
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
	MX_USART3_UART_Init();
  MX_TIM3_Init();
	printf("\r\nStarting...");

 	printf("\r\nRTC_done!");
	
//	HAL_RTC_GetTime(&RtcHandle, &Curent_time, RTC_FORMAT_BIN);
	printf("Time: %d:%d:%02d",Current_time.Hours, Current_time.Minutes, Current_time.Seconds);
	
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx1_buffer, 256);
	MX_RTC_Init();
	printf("\r\nRTC_INIT_EXIT!");
	//LCD	
	GLcd_Init(glcd_lcd_write_pin, 0);
	printf("\r\nLCD_INIT_EXIT!");
//	GLcd_SetFont();
	HAL_RTC_GetTime(&hrtc, &Current_time, RTC_FORMAT_BIN);
  GLcd_DrawString("Starting...", 0, 0, WHITE); /* Hien thi len man hinh GLCD trang thai khoi tao GSM/GPRS */
	sprintf(s,"%d:%d:%02d",Current_time.Hours, Current_time.Minutes, Current_time.Seconds);
	GLcd_DrawString(s, 0, 10, WHITE);	
	GLcd_Flush();
//	test_nv11();
	ssp_serial_initialize();
  ITLSSP_Init();
	int8_t i8_nv11_init = vdm_NV11_Init();
	if(i8_nv11_init == INIT_OK) printf("NV11 Init Successful!\r\n");
	else printf("NV11 Init Fail: %d\r\n",i8_nv11_init);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	uint8_t u8_nv11_proc;
	printf("Infinite loop\r\n");
	
	
  while (1)
  {		
		//------------------nv11 poll 500ms-----------------------------------
		vdm_NV11_Process(1000);
	
		//
		if(tim1_cnt>124)
		{
//			printf("\r\n[%d:%d:%d]\r\n",Curent_time.Hours, Curent_time.Minutes, Curent_time.Seconds+1);
						/* Get the RTC current Time */
			HAL_RTC_GetTime(&hrtc, &Current_time, RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc, &Current_date, RTC_FORMAT_BIN);
			/* Get the RTC current Date */
//			HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
			/* Display time Format : hh:mm:ss */
		//  sprintf((char*)showtime,"%02d:%02d:%02d",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
			tim1_cnt = 0;
			//printf("timer1_cnt %d\r\n",tim1_cnt);
			sprintf(s,"%s/%02d/%d %d:%02d:%02d", day[Current_date.Date], Current_date.Month, 2000 + Current_date.Year, Current_time.Hours, Current_time.Minutes, Current_time.Seconds);
//			Curent_time.Seconds++;
			GLcd_ClearScreen(BLACK);
			GLcd_DrawString(s, 0, 10, WHITE);
			GLcd_Flush();
			printf("\r\nRTC: %s", s);//"\r\nRTC: %d, %d, %d %d:%d:%d\r\n", Current_date.Date, Current_date.Month, 2000 + Current_date.Year, Current_time.Hours, Current_time.Minutes, Current_time.Seconds);
		}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		if(time_led_delay_ms)
//		{
//			HAL_Delay(time_led_delay_ms);
//			HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
//			HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
//		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }	
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 7;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}
/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_2;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_2;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */

  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);		

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BT_KEY0_Pin */
  GPIO_InitStruct.Pin = BT_KEY0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BT_KEY0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BT_WKUP_Pin */
  GPIO_InitStruct.Pin = BT_WKUP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BT_WKUP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED0_Pin */
  GPIO_InitStruct.Pin = LED0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);
	
  /*Configure GPIO pin : BUZZ_Pin */
  GPIO_InitStruct.Pin = BUZZ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZ_GPIO_Port, &GPIO_InitStruct);
	
	
  /*Configure GPIO pin : BUZZ_Pin */
  GPIO_InitStruct.Pin = CONVEYER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CONVEYER_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(CONVEYER_GPIO_Port, CONVEYER_Pin, GPIO_PIN_SET);
	
	

  /*Configure GPIO pin : BT_KEY0_Pin */
  GPIO_InitStruct.Pin = LCD_CS_PIN|LCD_CLK_PIN|LCD_DATA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);\
  /*Configure GPIO pin : RESSET_Pin */
  GPIO_InitStruct.Pin = LCD_RESET_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LCD_RESET_PORT, &GPIO_InitStruct);
	
	 HAL_GPIO_WritePin(LCD_RESET_PORT, LCD_RESET_PIN, GPIO_PIN_SET);	
	
  HAL_GPIO_WritePin(BUZZ_GPIO_Port, BUZZ_Pin, GPIO_PIN_SET);
	HAL_Delay(500);	
	HAL_GPIO_WritePin(BUZZ_GPIO_Port, BUZZ_Pin, GPIO_PIN_RESET);	
/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);
	if(htim->Instance == htim1.Instance)
	{
		tim1_cnt ++;
	}
}
/* USER CODE END 4 */

/* USER CODE 5 */
/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x06;
  sTime.Minutes = 0x30;
  sTime.Seconds = 0x55;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 0x01;
  DateToUpdate.Year = 0x24;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}
/* USER CODE END 5 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
