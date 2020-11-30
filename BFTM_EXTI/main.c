// Example of BFTM
// LED0 and LED1 will blink at frequency of 0.5Hz

#include "ht32f5xxxx_01.h"              // Device header

int led_on = 0, a = 8;

void BFTM0_IRQHandler(void)
{
	// your code
	
	HT_BFTM0->SR = 0;							// clear the BFTM interrupt flag
}

void EXTI0_1_IRQHandler(void)
{
	if((HT_EXTI->EDGEFLGR & (1<<1)))
	{
		if((HT_EXTI->EDGESR & (1<<1)))
			a = a + 1;
	}
	// Write 1 to clear both edge detection flag                   
	HT_EXTI->EDGEFLGR = 0xFFFF;
	// Write 1 to clear positive edge detection flag                                            
	//HT_EXTI->EDGESR = 0xFFFF;
}

void EXTI2_3_IRQHandler(void)
{
	a = a - 1;
	HT_EXTI->EDGEFLGR = 0xFFFF;
	// Write 1 to clear positive edge detection flag                                            
	//HT_EXTI->EDGESR = 0xFFFF;
}

int main(void)
{
	// Enable clock for GPIO, BFTM0, EXTI
	HT_CKCU->AHBCCR |= (1<<18);   	// PORTC clock
	HT_CKCU->AHBCCR |= (1<<19);   	// PORTD clock enable
	HT_CKCU->APBCCR1 |= (1<<16);  	// BFTM0 clock
	HT_CKCU->APBCCR0 |= (1<<15);	// EXTI clock
	HT_CKCU->APBCCR0 |= (1<<14);	// AFIO clock
	
	// Configure LED pins as output
	HT_GPIOC->DIRCR |= (1<<14);   	// PC14 is output
	HT_GPIOC->DIRCR |= (1<<15);		// PC15 is output
	HT_GPIOC->DOUTR = 0xFFFF;		// turn-off the LEDs
	
	// Configure PD1, PD2 as input pins
	HT_GPIOD->DIRCR &= ~(1<<1);
	HT_GPIOD->DIRCR &= ~(1<<2);
	HT_GPIOD->INER |= (3<<1); 
	
	// Assign EXTI to input pins
	HT_AFIO->ESSR[0] |= (3<<4);  //EXTI1
	HT_AFIO->ESSR[0] |= (3<<8);	 //EXTI2
	
	// Configure button EXTI Line on raising edge
	HT_EXTI->CFGR1 |= (3<<28);
	HT_EXTI->CFGR2 |= (3<<28);
	HT_EXTI->CR |= (3<<1);  			// enable EXTI interrupt
	
	// Select BFTM APB prescaler
	HT_CKCU->APBPCSR0 |= (3<<12); // PCLK = CK_AHB/8 = 6MHz
	// Configure BFTM timer: counter, reload value, enable timer, interrupt
	HT_BFTM0->CNTR = 0;						// reset counter
	HT_BFTM0->CMP = 96000;      // 16ms timer
	HT_BFTM0->CR = 5;					// enable timer, interrupt, repetitive mode
	
	// Enable EXTI, BFTM0 Interrupt
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	NVIC_EnableIRQ(EXTI2_3_IRQn);	
	NVIC_EnableIRQ(BFTM0_IRQn);
	
	while(1)
	{
		
	}	
}
