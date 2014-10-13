/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2013 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "STM32F4xx.h"

#include "dht11.h"

struct DHT11_Dev dev;
/*struct __FILE { int handle; };
FILE __stdout;
FILE __stdin;*/
volatile uint32_t msTicks;                      /* counts 1ms timeTicks       */

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;
}

/*----------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *----------------------------------------------------------------------------*/
void Delay (uint32_t dlyTicks) {                                              
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/*----------------------------------------------------------------------------
 Redirect printf
*----------------------------------------------------------------------------*/
/*int fputc(int ch, FILE *f) {
	ITM_SendChar(ch);
  return(ch);
}*/

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
	
  SystemCoreClockUpdate();                      /* Get Core Clock Frequency   */
  if (SysTick_Config(SystemCoreClock / 1000)) { /* SysTick 1 msec interrupts  */
    while (1);                                  /* Capture error              */
  }

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  //printf("startup...\n");
  DHT11_init(&dev, GPIOB, GPIO_Pin_6);
	
  while(1) {         
		int res = DHT11_read(&dev);
		/*if(res == DHT11_ERROR_CHECKSUM) {
			printf("ERROR\n");
		}
		else if(res == DHT11_SUCCESS) {
			printf("dht11 success\n");
			printf("T %d - H %d\n", dev.temparature, dev.humidity);
		}
		else {
			printf("TIMEOUT\n");
		}*/
		
		Delay(1000);
  }
}
