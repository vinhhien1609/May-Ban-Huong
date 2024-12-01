
//#include "pin_mux.h"
#include "serialfunc.h"
//#include "fsl_lpuart.h"
//#include "fsl_debug_console.h"
//#include "peripherals.h"
//#include "board.h"
#include "lwrb.h"
//#include "app_debug.h"
#include <stdio.h>
#include <string.h>
#include "GSM_drv.h"

#define SSP_PORT                    	1       /* Serial port used to communicate with NV11 */
#define SSP_BAUD	                	9600
#define SSP_UART				    	LPUART2 // LPUART_ReceiveEDMA(SSP_UART, &LPUART6_LPUART_eDMA_Handle, &rx_xfer);
#define SSP_IRQ_HANDLE			    	LPUART2_IRQHandler
#define SSP_IRQn				    	LPUART2_IRQn
#define SSP_DMA_RX_BUFFER_SIZE      	256
#define EVENT_GROUP_TX_DONE_BIT         (1 << 0)

//static AT_NONCACHEABLE_SECTION_INIT(uint8_t m_ssp_rx_raw_buffer[SSP_DMA_RX_BUFFER_SIZE+SSP_DMA_RX_BUFFER_SIZE]);
//static AT_NONCACHEABLE_SECTION_INIT(uint8_t m_ssp_rx_dma_buffer[SSP_DMA_RX_BUFFER_SIZE]);

extern DMA_HandleTypeDef hdma_usart2_rx;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
uint16_t rx1_cnt, rx1_size;
uint8_t rx1_buffer[256];
extern uint16_t topQueue, bottomQueue;
extern uint8_t GSM_buffer[256];
uint8_t GSM_buffer_DMA[256];

static lwrb_t m_ringbuffer_ssp_rx;


uint8_t m_ssp_rx_raw_buffer[SSP_DMA_RX_BUFFER_SIZE+SSP_DMA_RX_BUFFER_SIZE];
uint8_t m_ssp_rx_dma_buffer[SSP_DMA_RX_BUFFER_SIZE];

//#if defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT
//	#include "fsl_dmamux.h"
//#endif

//#include "fsl_lpuart_edma.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "event_groups.h"
//#include "app_debug.h"

//static EventGroupHandle_t m_event_group;
static volatile uint32_t m_dma_over_flow = 0U;
//static uint32_t remain = 0;

///* LPUART EDMA TX user callback */
//void ssp_uart_cb(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData)
//{
//    if (kStatus_LPUART_TxIdle == status)
//    {
//        BaseType_t m_ctx_sw;
//        xEventGroupSetBitsFromISR(m_event_group, EVENT_GROUP_TX_DONE_BIT, &m_ctx_sw);
//        if (m_ctx_sw)
//        {
//            portYIELD_FROM_ISR(m_ctx_sw);
//        }
//    }
//    else if (kStatus_LPUART_RxIdle == status)
//    {
//    	// Restart DMA RX
//        m_dma_over_flow++;
//        /* Disable LPUART RX EDMA. */
//        LPUART_EnableRxDMA(SSP_UART, false);
//        /* Stop transfer. */
//        EDMA_AbortTransfer(LPUART6_LPUART_eDMA_Handle.rxEdmaHandle);
//        LPUART6_LPUART_eDMA_Handle.rxState = 2; // (uint8_t)kLPUART_RxIdle;

//        // Restart DMA
//        lpuart_transfer_t rx_xfer;
//        rx_xfer.data = (uint8_t*)m_ssp_rx_dma_buffer;
//        rx_xfer.dataSize = SSP_DMA_RX_BUFFER_SIZE;
//        LPUART_ReceiveEDMA(SSP_UART, &LPUART6_LPUART_eDMA_Handle, &rx_xfer);
//    }
//}

//void SSP_IRQ_HANDLE()
//{
//    uint32_t status = LPUART_GetStatusFlags(SSP_UART);
//    uint32_t enable_isr = LPUART_GetEnabledInterrupts(SSP_UART);
//        /* If new data arrived. */
//    if ((0U != ((uint32_t)kLPUART_IdleLineFlag & status)) &&
//        (0U != ((uint32_t)kLPUART_IdleLineInterruptEnable & enable_isr)))
//    {
//        (void)LPUART_ClearStatusFlags(SSP_UART, kLPUART_IdleLineFlag);
//
//        uint32_t rx_bytes = SSP_DMA_RX_BUFFER_SIZE -
//                        			EDMA_GetRemainingMajorLoopCount(DMA0, LPUART6_RX_DMA_CHANNEL);
//        lwrb_write(&m_ringbuffer_ssp_rx, m_ssp_rx_dma_buffer, rx_bytes);

//        /* Disable LPUART RX EDMA. */
//        LPUART_EnableRxDMA(SSP_UART, false);
//        /* Stop transfer. */
//        EDMA_AbortTransfer(LPUART6_LPUART_eDMA_Handle.rxEdmaHandle);
//        LPUART6_LPUART_eDMA_Handle.rxState = 2; // (uint8_t)kLPUART_RxIdle;

//        // Restart DMA
//        lpuart_transfer_t rx_xfer;
//        rx_xfer.data = (uint8_t*)m_ssp_rx_dma_buffer;
//        rx_xfer.dataSize = SSP_DMA_RX_BUFFER_SIZE;
//        LPUART_ReceiveEDMA(SSP_UART, &LPUART6_LPUART_eDMA_Handle, &rx_xfer);
//    }

//    if ((kLPUART_RxOverrunFlag) & status)
//    {
//        // LPUART_ClearStatusFlags(SSP_UART, kLPUART_RxOverrunFlag);
//        SSP_UART->STAT = ((SSP_UART->STAT & 0x3FE00000U) | LPUART_STAT_OR_MASK);
////        SEGGER_RTT_Write(0, "SSP : Overrun\r\n");
//    }
//    else if ((kLPUART_NoiseErrorFlag) & status)
//    {
//        LPUART_ClearStatusFlags(SSP_UART, kLPUART_NoiseErrorFlag);
////        SEGGER_RTT_Write(0, "SSP : Noise\r\n");
//    }
//    else if ((kLPUART_FramingErrorFlag) & status)
//    {
//        LPUART_ClearStatusFlags(SSP_UART, kLPUART_FramingErrorFlag);
////        SEGGER_RTT_Write(0, "SSP : Frame error\r\n");
//    }
////    else if ((kLPUART_ParityErrorFlag) & status)		// parity none
////    {
////        LPUART_ClearStatusFlags(SSP_UART, kLPUART_ParityErrorFlag);
//////        SEGGER_RTT_Write(0, "SSP : Parity error\r\n");
////    }

//    if ((kLPUART_TransmissionCompleteFlag) & status)
//    {
//        LPUART_TransferEdmaHandleIRQ(SSP_UART, &LPUART6_LPUART_eDMA_Handle);
//    }
//    SDK_ISR_EXIT_BARRIER;
//}

//void ssp_serial_initialize(void)
//{
//	if (m_event_group == NULL)
//	{
//		m_event_group = xEventGroupCreate();
//	}
//	lwrb_init(&m_ringbuffer_ssp_rx, m_ssp_rx_raw_buffer, sizeof(m_ssp_rx_raw_buffer));

//    /* Enable RX interrupt for detecting the IDLE line interrupt. */
//    LPUART_EnableInterrupts(SSP_UART, kLPUART_IdleLineInterruptEnable);

//    lpuart_transfer_t rx_xfer;
//    rx_xfer.data = (uint8_t*)m_ssp_rx_dma_buffer;
//    rx_xfer.dataSize = SSP_DMA_RX_BUFFER_SIZE;
//    LPUART_ReceiveEDMA(SSP_UART, &LPUART6_LPUART_eDMA_Handle, &rx_xfer);
//    NVIC_SetPriority(SSP_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
//    EnableIRQ(SSP_IRQn);
//}

void ssp_serial_initialize(void)
{
	lwrb_init(&m_ringbuffer_ssp_rx, m_ssp_rx_raw_buffer, sizeof(m_ssp_rx_raw_buffer));
	
	/* DMA UART init :(uint8_t*)m_ssp_rx_dma_buffer; SSP_DMA_RX_BUFFER_SIZE */
	
	/* Enable RX interrupt for detecting the IDLE line interrupt. */
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, m_ssp_rx_dma_buffer, SSP_DMA_RX_BUFFER_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
}

void ssp_serial_deinitialize(void)
{
//    /* Disable interrupt. */
//    DisableIRQ(SSP_IRQn);
//    LPUART_DisableInterrupts(SSP_UART, kLPUART_IdleLineInterruptEnable);
//    // LPUART_Deinit(SSP_UART);
//    LPUART_TransferAbortReceiveEDMA(SSP_UART, &LPUART6_LPUART_eDMA_Handle);
    lwrb_reset(&m_ringbuffer_ssp_rx);
}

void ssp_serial_flush(void)
{
	lwrb_reset(&m_ringbuffer_ssp_rx);
}


void ssp_serial_write_data(const uint8_t * data, uint32_t length)
{
//    lpuart_transfer_t send_xfer;
//    /* Send g_tipString out. */
//    send_xfer.data = (uint8_t*)data;
//    send_xfer.dataSize = length;
//    if (kStatus_Success != LPUART_SendEDMA(SSP_UART, &LPUART6_LPUART_eDMA_Handle, &send_xfer))
//    {
//        DEBUG_ERROR("Failed to send uart DMA to SPP\r\n");
//    }
//    else
//    {
//        xEventGroupWaitBits(m_event_group, EVENT_GROUP_TX_DONE_BIT, pdTRUE, pdFALSE, 100);
//    }
	
//	printf(">> ");
//	for(uint32_t i=0;i<length;i++) printf("%02X ",data[i]);
//	printf(">>\r\n");
	
	HAL_UART_Transmit_DMA(&huart2,data,length);
}

uint32_t ssp_serial_rx_availble(void)
{
	if (m_dma_over_flow)
	{
		//DEBUG_ERROR("SSP RX buffer for DMA overflow\r\n"); //*\\duclq
		m_dma_over_flow = 0;
	}
    return lwrb_get_full(&m_ringbuffer_ssp_rx);;
}

uint32_t ssp_serial_read(uint8_t *buffer, uint32_t bytes_to_read)
{
	return lwrb_read(&m_ringbuffer_ssp_rx, buffer, bytes_to_read);
}

/* mUSER CODE BEGIN 1 */

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
	if(huart->Instance == huart2.Instance)
	{
//		HAL_GPIO_TogglePin(MCU_LCD_LIGHT_GPIO_Port,MCU_LCD_LIGHT_Pin);
	}
	if(huart->Instance == huart1.Instance)
	{
		
	}
	if(huart->Instance == huart3.Instance)
	{

	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	int16_t temp;
  UNUSED(huart);
  UNUSED(Size);
	if(huart->Instance == huart2.Instance)		//SSP
	{
//		HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		uint32_t rx_bytes = Size;
		
//			buff[3*rx_bytes] = 0x0d; buff[3*rx_bytes+1] = 0x0a;
//			for(uint32_t i=0;i<rx_bytes;i++)
//			{
//				buff[3*i] = ((m_ssp_rx_dma_buffer[i] & 0xf0) >> 4); if(buff[3*i] < 0x0a) buff[3*i] += '0'; else buff[3*i] += 'a' - 0x0a;
//				buff[3*i+1] = (m_ssp_rx_dma_buffer[i] & 0x0f); if(buff[3*i+1] < 0x0a) buff[3*i+1] += '0'; else buff[3*i+1] += 'a' - 0x0a;
//				buff[3*i+2] = ' ';
//			}
//			HAL_UART_Transmit_DMA(&huart1, buff, 3*rx_bytes+2);
		
		lwrb_write(&m_ringbuffer_ssp_rx, m_ssp_rx_dma_buffer, rx_bytes);

	//	/* Disable LPUART RX EDMA. */
	//	LPUART_EnableRxDMA(SSP_UART, false);
		
	//	/* Stop transfer. */
	//	EDMA_AbortTransfer(LPUART6_LPUART_eDMA_Handle.rxEdmaHandle);
	//	LPUART6_LPUART_eDMA_Handle.rxState = 2; // (uint8_t)kLPUART_RxIdle;

	//	// Restart DMA
	//	lpuart_transfer_t rx_xfer;
	//	rx_xfer.data = (uint8_t*)m_ssp_rx_dma_buffer;
	//	rx_xfer.dataSize = SSP_DMA_RX_BUFFER_SIZE;
	//	LPUART_ReceiveEDMA(SSP_UART, &LPUART6_LPUART_eDMA_Handle, &rx_xfer);
		
		HAL_UARTEx_ReceiveToIdle_DMA(&huart2, m_ssp_rx_dma_buffer, SSP_DMA_RX_BUFFER_SIZE);
		m_dma_over_flow++;
	}
	if(huart->Instance == huart1.Instance)	//debug
	{
//		rx1_cnt++;
//		if(Size<256) rx1_size = Size;
//		else rx1_size = 255;
		temp = rx1_size;
		rx1_size = Size;
//			buff[3*rx1_size] = 0x0d; buff[3*rx1_size+1] = 0x0a;
//			for(uint32_t i=0;i<rx1_size;i++)
//			{
//				buff[3*i] = ((rx1_buffer[i] & 0xf0) >> 4); if(buff[3*i] < 0x0a) buff[3*i] += '0'; else buff[3*i] += 'a' - 0x0a;
//				buff[3*i+1] = (rx1_buffer[i] & 0x0f); if(buff[3*i+1] < 0x0a) buff[3*i+1] += '0'; else buff[3*i+1] += 'a' - 0x0a;
//				buff[3*i+2] = ' ';
//			}
//			HAL_UART_Transmit_DMA(&huart1, buff, 3*rx1_size+2);
				HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx1_buffer, 256);

		
		if(temp< rx1_size)
			HAL_UART_Transmit_DMA(&huart3,rx1_buffer + temp, rx1_size-temp);
		else
		{
			HAL_UART_Transmit_DMA(&huart3,rx1_buffer + temp, 256-temp);
			HAL_UART_Transmit_DMA(&huart3,rx1_buffer, rx1_size);
		}
	}
	if(huart->Instance == huart3.Instance)		//GSM
	{
//		for(n=0; n<sizeof(GSM_buffer_DMA); n++)
//		{
//			GSM_buffer_DMA[n]=NULL;
//		}
		temp = bottomQueue;
		bottomQueue =Size;
		HAL_UARTEx_ReceiveToIdle_DMA(&huart3, GSM_buffer, MAX_QUEUE);

//		if(bottomQueue >=MAX_QUEUE)	bottomQueue-=MAX_QUEUE;
//		for(n=0; n<256; n++)
//		{
//			if(GSM_buffer_DMA[n]==0)	break;
//			GSM_buffer[botqueue] = GSM_buffer_DMA[n];
//			botqueue ++;
//			if(botqueue>=MAX_QUEUE)	botqueue =0;
//		}
		if(temp< bottomQueue)
			HAL_UART_Transmit_DMA(&huart1,GSM_buffer + temp, bottomQueue-temp);
		else
		{
			HAL_UART_Transmit_DMA(&huart1,GSM_buffer + temp, MAX_QUEUE-temp);
			HAL_UART_Transmit_DMA(&huart1,GSM_buffer, bottomQueue);
		}
			
	}
}
/* mUSER CODE END 1 */
