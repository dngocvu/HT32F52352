// Example of BFTM
// LED0 and LED1 will blink at frequency of 0.5Hz

#include "ht32f5xxxx_01.h"              // Device header

int led_on = 0;

void BFTM0_IRQHandler(void)
{
	HT_GPIOC->DOUTR = ~HT_GPIOC->DOUTR;  // toggle den
	HT_BFTM0->SR = 0;							// clear the BFTM interrupt flag
}

int main(void)
{
	// Enable clock for GPIO, BFTM0
	HT_CKCU->AHBCCR |= (1<<18);   // PORTC clock
	HT_CKCU->APBCCR1 |= (1<<16);  // BFTM0 clock
	// Configure LED pins as output
	HT_GPIOC->DIRCR |= (1<<14);   // PC14 is output
	HT_GPIOC->DIRCR |= (1<<15);		// PC15 is output
	HT_GPIOC->DOUTR = 0xFFFF;				// turn-off the LEDs
	// Select BFTM APB prescaler
	HT_CKCU->APBPCSR0 |= (3<<12); // PCLK = CK_AHB/8 = 6MHz
	// Configure BFTM timer: counter, reload value, enable timer, interrupt
	HT_BFTM0->CNTR = 0;						// reset counter
	HT_BFTM0->CMP = 960000;      // 160ms timer
	HT_BFTM0->CR = 5;					// enable timer, interrupt, repetitive mode
	//NVIC enable 
	NVIC_EnableIRQ(BFTM0_IRQn);
	
	while(1)
	{
		
	}	
}
