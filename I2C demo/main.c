#include "ht32f5xxxx_01.h"              // Device header

uint8_t signal = 0,i;

// EXTERNAL INTERRUPT
void EXTI0_1_IRQHandler(void) // Handle for EXTI_1
{	
	HT_EXTI->EDGEFLGR |=(1<<1); // Clear interrupt flag of EXTI1
}
//
void init(void)
{
	
	// Enable SPI clock for I2C
	HT_CKCU -> APBCCR0 |= (1 << 1);
	// Specifies the APB clock prescaler for SPI
	HT_CKCU -> APBPCSR0 |= (2<<2);//  AHB/4
	// Enable clock for AFIO
	HT_CKCU -> APBCCR0 |= (1 << 14);
	// Set up AF7 for I2C
	HT_AFIO -> GPBCFGR[1] |= (1 << 16);
	// Specifies the i2c addressing mode, Stop bit, ACK bit, enable I2C in I2CCR register
	
	// Assign address to I2C devices in I2CADDR and I2CTAR registers
	
	// Specifies the I2C clock period in I2CSHPGR/I2CSLPGR registers
	
	// Enable I2C interrupts (optional) in I2CIER register
	
	// Config as input
	HT_GPIOD -> DIRCR &=~ (3 << 1);
	HT_GPIOB -> DIRCR &=~ (1 << 12);
	HT_GPIOD->INER = 0xFFFF;	// initial value
	HT_GPIOB->INER = 0xFFFF;	// initial value
	// Config as output
	HT_GPIOC -> DIRCR |= (3 << 14); // PC14/LED0;PC15/LED1
	HT_GPIOC -> DIRCR |= (1 << 1);
	HT_GPIOC -> DOUTR = 0xFFFF;  // initial value
	//Assign EXTI function to GPIO pins
	HT_AFIO->ESSR[0] |= (3 << 4);  // PD1-Button1-EXTI1
	//Configure rising-edge detection
	HT_EXTI->CFGR1 |= (4 << 28);	// Positive edge Trigger for EXT1
	// Enable EXTI interrupt pins
	HT_EXTI->CR |= (3 << 1);  // Enable for EXTI1, EXTI2
	// Select BFTM APB prescaler
	HT_CKCU->APBPCSR0 |= (3<<12); // PCLK = CK_AHB/8 = 6MHz	
	// Configure BFTM timer: counter, reload value, enable timer, interrupt
	HT_BFTM0->CNTR = 0;						// reset counter
	HT_BFTM0->CMP = 96000;     // 16ms timer
	HT_BFTM0->CR = 5;					// enable timer, interrupt, repetitive mode
	
	// Enable EXTI interrupts
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	
}

int main(void)
{
	init();
	while(1)
	{
		// empty
	}
}