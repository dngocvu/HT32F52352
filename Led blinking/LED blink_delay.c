/**************************************************************************************
 * @file    Led blink/main.c
 * @version $Rev::           $
 * @date    $Date::  #$
 * @brief   Main program.
 *************************************************************************************/
#include "ht32f5xxxx_01.h"              // Device header
//static vu32 DelayTime;
/* Counts 1ms timeTicks */
volatile uint32_t msTicks = 0;
 
void Delay(uint32_t dlyTicks)
{
      uint32_t curTicks;
 
      curTicks = msTicks;
      while ((msTicks - curTicks) < dlyTicks) ;
}
 
void SysTick_Handler(void)
{
      /* Increment counter necessary in Delay()*/
      msTicks++;
}

int main(void)
{
	// Enable Clock for PORT C in AHBCCR register
	 HT_CKCU->AHBCCR |= (1<<18);  //enable Port C clock
	
	// Configure PC1, PC14, PC15 as output pins
	 HT_GPIOC->DIRCR |= (1<<1);
	 HT_GPIOC->DIRCR |= (1<<14);
	 HT_GPIOC->DIRCR |= (1<<15);
	 HT_GPIOC->DOUTR = 0xFFFF;  // initial value
	

  // SYSTICK configuration 
		SysTick_Config(48000); 			// Default : CK_SYS/8
																	// (CK_SYS/8/1000) = 1ms on chip
																	// Enable SYSTICK Interrupt

  /* Infinite loop */
  while(1)
  {
		HT_GPIOC->RR |= (1 << 1);   // reset pin PC1
		Delay(500);
		HT_GPIOC->SRR |= (1 << 1);   // reset pin PC1
		Delay(500);
		
  }
}


