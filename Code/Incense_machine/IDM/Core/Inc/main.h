/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern uint32_t time_buzz;
#define Buzz_On(x)	{time_buzz = x; HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);};
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MOTOR_CURRENT_Pin GPIO_PIN_0
#define MOTOR_CURRENT_GPIO_Port GPIOC
#define SWAP_CURRENT_Pin GPIO_PIN_1
#define SWAP_CURRENT_GPIO_Port GPIOC
#define BUZZER_Pin GPIO_PIN_3
#define BUZZER_GPIO_Port GPIOC
#define DHT21_Pin GPIO_PIN_0
#define DHT21_GPIO_Port GPIOA
#define Heater_Pin GPIO_PIN_1
#define Heater_GPIO_Port GPIOA
#define MCU_NV9_TX_Pin GPIO_PIN_2
#define MCU_NV9_TX_GPIO_Port GPIOA
#define MCU_NV9_RX_Pin GPIO_PIN_3
#define MCU_NV9_RX_GPIO_Port GPIOA
#define Flash_MOSI_Pin GPIO_PIN_4
#define Flash_MOSI_GPIO_Port GPIOA
#define Flash_MISO_Pin GPIO_PIN_5
#define Flash_MISO_GPIO_Port GPIOA
#define Flash_SCK_Pin GPIO_PIN_6
#define Flash_SCK_GPIO_Port GPIOA
#define Flash_SS_Pin GPIO_PIN_7
#define Flash_SS_GPIO_Port GPIOA
#define Empty_Pin GPIO_PIN_4
#define Empty_GPIO_Port GPIOC
#define Drop_Pin GPIO_PIN_5
#define Drop_GPIO_Port GPIOC
#define Drop_EXTI_IRQn EXTI9_5_IRQn
#define DOOR_Pin GPIO_PIN_0
#define DOOR_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_1
#define LED3_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_7
#define LED1_GPIO_Port GPIOE
#define GSM_RESET_Pin GPIO_PIN_8
#define GSM_RESET_GPIO_Port GPIOE
#define GSM_PWKEY_Pin GPIO_PIN_9
#define GSM_PWKEY_GPIO_Port GPIOE
#define EN_PWR_GSM_Pin GPIO_PIN_10
#define EN_PWR_GSM_GPIO_Port GPIOE
#define RELAY5_Pin GPIO_PIN_11
#define RELAY5_GPIO_Port GPIOE
#define RELAY4_Pin GPIO_PIN_12
#define RELAY4_GPIO_Port GPIOE
#define RELAY3_Pin GPIO_PIN_13
#define RELAY3_GPIO_Port GPIOE
#define RELAY2_Pin GPIO_PIN_14
#define RELAY2_GPIO_Port GPIOE
#define RELAY1_Pin GPIO_PIN_15
#define RELAY1_GPIO_Port GPIOE
#define GSM_TX_Pin GPIO_PIN_10
#define GSM_TX_GPIO_Port GPIOB
#define GSM_RX_Pin GPIO_PIN_11
#define GSM_RX_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOB
#define LCD_RESET_Pin GPIO_PIN_8
#define LCD_RESET_GPIO_Port GPIOD
#define LCD_INT_Pin GPIO_PIN_9
#define LCD_INT_GPIO_Port GPIOD
#define KEY_ROW3_Pin GPIO_PIN_10
#define KEY_ROW3_GPIO_Port GPIOD
#define KEY_ROW4_Pin GPIO_PIN_11
#define KEY_ROW4_GPIO_Port GPIOD
#define KEY_ROW1_Pin GPIO_PIN_12
#define KEY_ROW1_GPIO_Port GPIOD
#define KEY_ROW2_Pin GPIO_PIN_13
#define KEY_ROW2_GPIO_Port GPIOD
#define KEY_COL3_Pin GPIO_PIN_14
#define KEY_COL3_GPIO_Port GPIOD
#define KEY_COL4_Pin GPIO_PIN_15
#define KEY_COL4_GPIO_Port GPIOD
#define KEY_COL1_Pin GPIO_PIN_6
#define KEY_COL1_GPIO_Port GPIOC
#define KEY_COL2_Pin GPIO_PIN_7
#define KEY_COL2_GPIO_Port GPIOC
#define MCU_LCD_LIGHT_Pin GPIO_PIN_8
#define MCU_LCD_LIGHT_GPIO_Port GPIOA
#define DEBUG_TX_Pin GPIO_PIN_9
#define DEBUG_TX_GPIO_Port GPIOA
#define MCU_IN1_Pin GPIO_PIN_5
#define MCU_IN1_GPIO_Port GPIOD
#define MCU_IN2_Pin GPIO_PIN_6
#define MCU_IN2_GPIO_Port GPIOD
#define MCU_IN3_Pin GPIO_PIN_7
#define MCU_IN3_GPIO_Port GPIOD
#define MCU_IN4_Pin GPIO_PIN_3
#define MCU_IN4_GPIO_Port GPIOB
#define MCU_IN5_Pin GPIO_PIN_4
#define MCU_IN5_GPIO_Port GPIOB
#define SOLENOID_Pin GPIO_PIN_5
#define SOLENOID_GPIO_Port GPIOB
#define SWAP_MOTOR_Pin GPIO_PIN_6
#define SWAP_MOTOR_GPIO_Port GPIOB
#define CONVEYER_MOTOR_Pin GPIO_PIN_7
#define CONVEYER_MOTOR_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
