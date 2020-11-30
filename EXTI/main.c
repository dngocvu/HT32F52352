#include "ht32f5xxxx_01.h"              // Device header

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

void EXTI0_1_IRQHandler(void)
{
	
}

void EXTI2_3_IRQHandler(void)
{
	
}

void EXTI4_15_IRQHandler(void)
{
	
}

int main(void)
{
	// Enable Clock for PORT C and PORTD in AHBCCR register
	// Enable Clock for EXTI and AFIO in APBCCR0 register

	 	 
	// Configure PC1, PC14, PC15 as output pins

	
	// Set AF1(GPIO) for PB12 - WakeUp button
 
	
	// Configure PB12, PD1, PD2 as input pins

	
	// Assign EXTI to input pins

	
	// Configure button EXTI Line on falling edge
	
	
	// Enable Interrupt

	
	
	
	// SYSTICK configuration 
	SysTick_Config(48000); 			// 1ms timer

  /* Infinite loop */
  while(1)
  {
	
		
  }
}
