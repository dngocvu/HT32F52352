// Example of GPTM
// Output PWM at pin PC10 - GT1_CH0

#include "ht32f5xxxx_01.h"              // Device header

void GPTM1_IRQHandler(void)
{
	HT_GPIOC->DOUTR = ~HT_GPIOC->DOUTR;
	HT_GPTM1->INTSR &= ~(1<<0);						// clear the CH0CCIF bit
	//HT_GPTM1->INTSR &= ~(2<<0);						// clear the CH1CCIF bit
	//HT_GPTM1->INTSR &= ~(3<<0);						// clear the CH2CCIF bit
}

int main(void)
{
	// Enable clock for GPIO, GPTM1, AFIO
	HT_CKCU->AHBCCR |= (1<<18);   	// PORTC clock
	HT_CKCU->APBCCR0 |= (1<<14);   	// AFIO clock
	HT_CKCU->APBCCR1 |= (1<<9);   	// GPTM1 clock
	// Configure LED pins as output
	HT_GPIOC->DIRCR |= (1<<14);   	// PC14 is output
	HT_GPIOC->DIRCR |= (1<<15);			// PC15 is output
	HT_GPIOC->DOUTR = 0xFFFF;				// turn-off the LEDs
									// PC10 ???
									// pin for encoder's channelA
									// Pin for encoder's channelB 
									
	// Assign AF4(GPTM) to PC10
	HT_AFIO->GPCCFGR[1] |= (4<<8);
	// 
	
	//
	
	// Select GPTM1 APB prescaler
	HT_CKCU->APBPCSR0 |= (3<<22); 	// PCLK = CK_AHB/8 = 6MHz
	
	// Configure GPTM PWM output: 
	HT_GPTM1->CNTCFR &= ~(1<<24); 	// counting up
	//HT_GPTM1->CNTCFR &= ~(3<<16);		// edge aligned mode
	HT_GPTM1->CH0ICFR = 0;  // CH0CCS = 00, Channel 0 is configured as an output
	HT_GPTM1->CH1ICFR |= (16<<1); // Channel 1 is configured as an input from TI1 signal
	HT_GPTM1->CH2ICFR |= (16<<1); // Channel 2 is configured as an input from TI2 signal
	HT_GPTM1->CH0OCFR |= (6<<0);		// PWM mode 1
	//HT_GPTM1->CHPOLR &= ~(1<<0);	// Channel 0 Output active high
	HT_GPTM1->CHCTR |= (1<<0);			// channel 0 output enable 
	HT_GPTM1->CNTR = 0;							// Reset counter
	HT_GPTM1->PSCR = 99;						// prescaler = 99 + 1
	HT_GPTM1->CH0CCR = 50000;				// 0.5s
	HT_GPTM1->CRR = 60000;					// 1s timer
	HT_GPTM1->DICTR |= (1<<0); 			// Channel 0 interrupt enabled
	//HT_GPTM1->CTR |= (1<<1);				// Counter reload register can not be updated until the update event occurs
	HT_GPTM1->CTR |= (1<<0);				// Enable GPTM timer  
	
	//NVIC enable 
	NVIC_EnableIRQ(GPTM1_IRQn);
	
	while(1)
	{
		
	}	
}
