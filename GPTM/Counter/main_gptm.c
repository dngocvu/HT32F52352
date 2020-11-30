// Example of GPTM
// LED0 and LED1 will blink at frequency of 0.5Hz

#include "ht32f5xxxx_01.h"              // Device header

void GPTM0_IRQHandler(void)
{
	HT_GPIOC->DOUTR = ~HT_GPIOC->DOUTR;
	HT_GPTM0->INTSR &= ~(1<<8);						// clear the GPTM update event interrupt flag
}

int main(void)
{
	// Enable clock for GPIO, GPTM0
	HT_CKCU->AHBCCR |= (1<<18);   		// PORTC clock
	HT_CKCU->APBCCR1 |= (1<<8);   		// GPTM0 clock
	// Configure LED pins as output
	HT_GPIOC->DIRCR |= (1<<14);   		// PC14 is output
	HT_GPIOC->DIRCR |= (1<<15);				// PC15 is output
	HT_GPIOC->DOUTR = 0xFFFF;					// turn-off the LEDs
	// Select GPTM0 APB prescaler
	HT_CKCU->APBPCSR0 |= (3<<20); 		// PCLK = CK_AHB/8 = 6MHz
	// Configure GPTM timer: counting up, reload value, enable timer, interrupt
	//HT_GPTM0->CNTCFR &= ~(1<<24); 	// counting up
	//HT_GPTM0->CNTCFR &= ~(3<<16);		// edge aligned mode
	//HT_GPTM0->CNTCFR |= (1<<1); 		// Only overflow generates interrupt
	//HT_GPTM0->CNTCFR &= ~(1<<0);		// Enable the update event
	HT_GPTM0->CNTR = 0;								// Reset timer
	HT_GPTM0->PSCR = 99;							// prescaler = 99 + 1
	HT_GPTM0->CRR = 60000;						// 1s timer
	HT_GPTM0->DICTR |= (1<<8); 				// Update event interrupt enable
	HT_GPTM0->CTR |= (1<<1);					// Counter reload register can not be updated until the update event occurs
	HT_GPTM0->CTR |= (1<<0);					// Enable GPTM timer  
	
	//NVIC enable 
	NVIC_EnableIRQ(GPTM0_IRQn);
	
	while(1)
	{
		
	}	
}
