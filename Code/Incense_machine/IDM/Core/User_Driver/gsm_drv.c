
#include "gsm_drv.h"


extern UART_HandleTypeDef huart3;
extern uint8_t GSM_buffer[];
void GSM_UART_init()
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3, GSM_buffer, MAX_QUEUE);
}
void GSM_PWR(GPIO_PinState pwr)
{
	HAL_GPIO_WritePin(EN_PWR_GSM_GPIO_Port, EN_PWR_GSM_Pin, pwr);
}
void GSM_On(void)
{
	GSM_PWR(ON);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GSM_PWKEY_GPIO_Port, GSM_PWKEY_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GSM_PWKEY_GPIO_Port, GSM_PWKEY_Pin, GPIO_PIN_RESET);
}
void GSM_Off(void)
{
	HAL_GPIO_WritePin(GSM_PWKEY_GPIO_Port, GSM_PWKEY_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GSM_PWKEY_GPIO_Port, GSM_PWKEY_Pin, GPIO_PIN_RESET);
	GSM_PWR(OFF);
}
