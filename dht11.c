#include "dht11.h"

int DHT11_init(struct DHT11_Dev* dev, GPIO_TypeDef* port, uint16_t pin) {
	TIM_TimeBaseInitTypeDef TIM_TimBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	dev->port = port;
	dev->pin = pin;

	//Initialise TIMER2
	TIM_TimBaseStructure.TIM_Period = 84000000 - 1;
	TIM_TimBaseStructure.TIM_Prescaler = 84;
	TIM_TimBaseStructure.TIM_ClockDivision = 0;
	TIM_TimBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimBaseStructure);
	TIM_Cmd(TIM2, ENABLE);
	
	//Initialise GPIO DHT11
	GPIO_InitStructure.GPIO_Pin = dev->pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(dev->port, &GPIO_InitStructure);
	
	return 0;
}

int DHT11_read(struct DHT11_Dev* dev) {
	
	//Initialisation
	uint8_t i, j, temp;
	uint8_t data[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Generate START condition
	//o
	GPIO_InitStructure.GPIO_Pin = dev->pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(dev->port, &GPIO_InitStructure);
	
	//dev->port->MODER |= GPIO_MODER_MODER6_0;
	
	//Put LOW for at least 18ms
	GPIO_ResetBits(dev->port, dev->pin);
	
	//wait 18ms
	TIM2->CNT = 0;
	while((TIM2->CNT) <= 18000);
	
	//Put HIGH for 20-40us
	GPIO_SetBits(dev->port, dev->pin);
	
	//wait 40us
	TIM2->CNT = 0;
	while((TIM2->CNT) <= 40);
	//End start condition
	
	//io();
	//Input mode to receive data
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(dev->port, &GPIO_InitStructure);
	
	//DHT11 ACK
	//should be LOW for at least 80us
	//while(!GPIO_ReadInputDataBit(dev->port, dev->pin));
	TIM2->CNT = 0;
	while(!GPIO_ReadInputDataBit(dev->port, dev->pin)) {
		if(TIM2->CNT > 100)
			return DHT11_ERROR_TIMEOUT;
	}
	
	//should be HIGH for at least 80us
	//while(GPIO_ReadInputDataBit(dev->port, dev->pin));		
	TIM2->CNT = 0;
	while(GPIO_ReadInputDataBit(dev->port, dev->pin)) {
		if(TIM2->CNT > 100)
			return DHT11_ERROR_TIMEOUT;
	}
	
	//Read 40 bits (8*5)
	for(j = 0; j < 5; ++j) {
		for(i = 0; i < 8; ++i) {
			
			//LOW for 50us
			while(!GPIO_ReadInputDataBit(dev->port, dev->pin));
			/*TIM2->CNT = 0;
			while(!GPIO_ReadInputDataBit(dev->port, dev->pin)) {
				if(TIM2->CNT > 60)
					return DHT11_ERROR_TIMEOUT;
			}*/
			
			//Start counter
			TIM_SetCounter(TIM2, 0);
			
			//HIGH for 26-28us = 0 / 70us = 1
			while(GPIO_ReadInputDataBit(dev->port, dev->pin));
			/*while(!GPIO_ReadInputDataBit(dev->port, dev->pin)) {
				if(TIM2->CNT > 100)
					return DHT11_ERROR_TIMEOUT;
			}*/
			
			//Calc amount of time passed
			temp = TIM_GetCounter(TIM2);
			
			//shift 0
			data[j] = data[j] << 1;
			
			//if > 30us it's 1
			if(temp > 40)
				data[j] = data[j]+1;
		}
	}
	
	//verify the Checksum
	if(data[4] != (data[0] + data[2]))
		return DHT11_ERROR_CHECKSUM;
	
	//set data
	dev->temparature = data[2];
	dev->humidity = data[0];
	
	return DHT11_SUCCESS;
}
