/*
 * dht_sensor.c
 *
 *  Created on: Mar 27, 2020
 *      Author: luighi
 */

#include "main.h"
#include "dht_sensor.h"
#include "IDM.h"

#define MIN_DELAY_INTERVAL 2000 /**< minimum delay between reads */
#define WAIT_SIGNAL_US 55
//#define MAX_WAIT_TIME_US 1000
#define MAX_WAIT_TIME_US 3000
#define TIMEOUT MAX_WAIT_TIME_US+10
long time_test=0;
bool flag_readDHT12=false;
/**
 * Reference: https://stackoverflow.com/a/60891394/5107192
 */
#define SINLINE static inline __attribute__((always_inline))
	
extern IDM_PARA IDM;

SINLINE void  DHT_CLK_ENABLE(GPIO_TypeDef * const gpio)
{
    switch((uint32_t)gpio)
    {
        case (uint32_t)GPIOA:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            break;
        case (uint32_t)GPIOB:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            break;
        case (uint32_t)GPIOC:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            break;
        case (uint32_t)GPIOD:
			__HAL_RCC_GPIOD_CLK_ENABLE();
			break;
        case (uint32_t)GPIOE:
			__HAL_RCC_GPIOE_CLK_ENABLE();
			break;
        case (uint32_t)GPIOF:
            __HAL_RCC_GPIOF_CLK_ENABLE();
            break;
        case (uint32_t)GPIOG:
			__HAL_RCC_GPIOG_CLK_ENABLE();
			break;
    }
}

float evalHumidity(uint8_t* data, uint8_t len, DHT_TypeDef type);
float evalTemperature(uint8_t* data, uint8_t len, DHT_TypeDef type);

void _delay(unsigned long t)
{
	while(t--);
}

void DHT_Sensor_Init(DHT_HandleTypeDef *hdht){

  GPIO_InitTypeDef  gpio_init_structure;

  DHT_CLK_ENABLE(DHT_GPIO_conf);

  gpio_init_structure.Pin   = DHT_PIN_conf;
  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_OD;
  gpio_init_structure.Pull  = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(DHT_GPIO_conf, &gpio_init_structure);
  HAL_GPIO_WritePin(DHT_GPIO_conf, DHT_PIN_conf, GPIO_PIN_SET);

  hdht->DHT_GPIO = DHT_GPIO_conf;
  hdht->DHT_PIN = DHT_PIN_conf;
  hdht->DHT_Type = DHT_Type_conf;
  hdht->lastReadTime = MIN_DELAY_INTERVAL;
}

DHT_ReadStatus DHT_Sensor_Read(DHT_HandleTypeDef *hdht){

//	uint32_t currentTime = MIN_DELAY_INTERVAL + MIN_DELAY_INTERVAL; //HAL_GetTick();
	GPIO_InitTypeDef  gpio_init_structure;
//	if ((currentTime - hdht->lastReadTime) < MIN_DELAY_INTERVAL) {
////		DEBUG_PRINT("Early read, time not passed, returned last read\n\r");
//		hdht->status = EARLY_READ;
//	    return hdht->status; // return last correct measurement
//	}

//	hdht->lastReadTime = currentTime;

	//set output
  gpio_init_structure.Pin   = DHT_PIN_conf;
  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_OD;
  gpio_init_structure.Pull  = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT_GPIO_conf, &gpio_init_structure);
	
	for(int i; i<DATA_BYTE_LENGTH;i++){
		hdht->data[i] = 0;
	}
	HAL_GPIO_WritePin(DHT_GPIO_conf, DHT_PIN_conf, GPIO_PIN_SET);
	_delay(4500);
	printf("DHT21 READ\r\n");
	flag_readDHT12=true;
	HAL_GPIO_WritePin(DHT_GPIO_conf, DHT_PIN_conf, GPIO_PIN_RESET);
	
	switch (hdht->DHT_Type) {
	case DHT22:
	case DHT21:	//>1.1ms
	_delay(5500);
		break;
	case DHT11:
	default:		//20ms
		//HAL_Delay(20); // data sheet says at least 18ms, 20ms just to be safe
	_delay(90000);
	break;
	}
	uint32_t cycles[80];
	{
	    // End the start signal by setting data line high
			HAL_GPIO_WritePin(DHT_GPIO_conf, DHT_PIN_conf, GPIO_PIN_SET);
		//set input
			gpio_init_structure.Pin   = DHT_PIN_conf;
			gpio_init_structure.Mode  = GPIO_MODE_INPUT;
			gpio_init_structure.Pull  = GPIO_NOPULL;
			gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

			HAL_GPIO_Init(DHT_GPIO_conf, &gpio_init_structure);		

	    // Delay a moment to let sensor pull data line low.
//	    _delay(WAIT_SIGNAL_US);
	    if (waitForPulse(1) == TIMEOUT) {
	      hdht->status = TIMEOUT_LOW;
	      return hdht->status;
	    }
//		 printf("----\r\n");			
	    // Now start reading the data line to get the value from the DHT sensor.

	    // First expect a low signal for ~80 microseconds followed by a high signal
	    // for ~80 microseconds again.
	    if (waitForPulse(0) == TIMEOUT) {
	      hdht->status = TIMEOUT_LOW;
	      return hdht->status;
	    }
	//		printf("read ready LOW\r\n");
	    if (waitForPulse(1) == TIMEOUT) {
	      hdht->status = TIMEOUT_HIGH;
	      return hdht->status;
	    }
	    // Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
	    // microsecond low pulse followed by a variable length high pulse.  If the
	    // high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
	    // then it's a 1.  We measure the cycle count of the initial 50us low pulse
	    // and use that to compare to the cycle count of the high pulse to determine
	    // if the bit is a 0 (high state cycle count < low state cycle count), or a
	    // 1 (high state cycle count > low state cycle count). Note that for speed
	    // all the pulses are read into a array and then examined in a later step.
	    for (int i = 0; i < 80; i += 2) {
	      cycles[i] = waitForPulse(0);
	      cycles[i + 1] = waitForPulse(1);
	    }
	  } // Timing critical code is now complete.
		printf("read ready 80 bits\r\n");
		flag_readDHT12 = false;
	  // Inspect pulses and determine which ones are 0 (high state cycle count < low
	  // state cycle count), or 1 (high state cycle count > low state cycle count).
	  for (int i = 0; i < 40; ++i) {
	    uint32_t lowCycle = cycles[2 * i];
	    uint32_t highCycle = cycles[2 * i + 1];
	    if ((lowCycle == TIMEOUT) || (highCycle == TIMEOUT)) {
	      hdht->status = TIMEOUT_CYCLES;
	      return hdht->status;
	    }
	    hdht->data[i / 8] <<= 1;
	    // Now compare the low and high cycle times to see if the bit is a 0 or 1.
	    if (highCycle > lowCycle) {
	      // High cycles are greater than 50us low cycle count, must be a 1.
	    	hdht->data[i / 8] |= 1;
	    }			
	    // Else high cycles are less than (or equal to, a weird case) the 50us low
	    // cycle count so this must be a zero.  Nothing needs to be changed in the
	    // stored data.
	  }
		
		printf("DHT21>> REG %d %d %d %d %d\r\n", hdht->data[0], hdht->data[1], hdht->data[2], hdht->data[3], hdht->data[4]);
	  uint8_t parity;
	  parity = (hdht->data[0] + hdht->data[1] + hdht->data[2] + hdht->data[3]) & 0xFF;
	  if(hdht->data[4]!=parity){
		  hdht->status = CHECKSUM_FAILED;
			printf("DHT21>> CHECKSUM_FAILED\r\n");
		  return hdht->status;
	  }
		
	  /*Evaluating the humidity and temperature*/

//	  hdht->values.temperature = evalTemperature(hdht->data,countof(hdht->data),hdht->DHT_Type);
//	  hdht->values.humidity= evalHumidity(hdht->data,countof(hdht->data),hdht->DHT_Type);
	  hdht->values.temperature = (float)(((uint16_t)(hdht->data[2] & 0x7F)) << 8 | hdht->data[3])*0.1;
		if(hdht->data[2] &0x80)
			hdht->values.temperature*= -1;
	  hdht->values.humidity= (float)(((uint16_t)hdht->data[0]) << 8 | hdht->data[1])*0.1;
		printf("DHT21>> HUM: %.02f\r\n", hdht->values.humidity);
	  hdht->status = READ_OK;
	  return hdht->status;
}

float evalHumidity(uint8_t* data, uint8_t len, DHT_TypeDef type)
{
	float f = 0;
	uint8_t _data[len];
	memcpy(_data,data,len);

	if(len > 2){
		switch (type) {
		case DHT11:
		case DHT12:
		  f = _data[0] + _data[1] * 0.1;
		  break;
		case DHT22:
		case DHT21:
		  f = ((uint16_t)_data[0]) << 8 | _data[1];
		  f *= 0.1;
		  break;
		}
	}

	return f;
}


float evalTemperature(uint8_t* data, uint8_t len, DHT_TypeDef type)
{
	 float f = 0;
	 uint8_t _data[len];
	 memcpy(_data,data,len);

	  if (len > 4) {
	    switch (type) {
	    case DHT11:
	      f = _data[2];
	      if (_data[3] & 0x80) {
	        f = -1 - f;
	      }
	      f += (_data[3] & 0x0f) * 0.1;

	      break;
	    case DHT12:
	      f = _data[2];
	      f += (_data[3] & 0x0f) * 0.1;
	      if (_data[2] & 0x80) {
	        f *= -1;
	      }

	      break;
	    case DHT22:
	    case DHT21:
	      f = ((uint16_t)(_data[2] & 0x7F)) << 8 | _data[3];
	      f *= 0.1;
	      if (_data[2] & 0x80) {
	        f *= -1;
	      }

	      break;
	    }
	  }
	  return f;
}



uint32_t waitForPulse(uint8_t level){

//	resetTick_us();
	uint32_t count_wait=0;

	while (HAL_GPIO_ReadPin(DHT_GPIO_conf, DHT_PIN_conf) == level) {
//	    if (getTick_us() >= MAX_WAIT_TIME_US) {
//				time_test = count_wait;
				if(count_wait++ >=MAX_WAIT_TIME_US)	{
				printf("HDT21>> Timeout\r\n");
				flag_readDHT12= false;
	      return TIMEOUT; // Exceeded timeout, fail.
	    }
	}
	//return getTick_us();
	return count_wait;
}


