#include "ht32f5xxxx_01.h"              // Device header


volatile uint32_t msTicks = 0;
uint16_t count = 0, i;
//
void Delay(uint32_t dlyTicks)
{
	uint32_t curTicks;
	
	curTicks = msTicks;
	while((msTicks-curTicks)<dlyTicks);
}
//
void SysTick_Handler(void)
{
	// Increment counter necessary in Delay()
	msTicks++;
}
//
void led_dimmer()
{
	unsigned char i,j;
 
	for(i=1;i<=12;i++)
	{
		j=0;
		while(++j<8)
		{   // tong time delay la 12*8 = 96 ms
			HT_GPIOC->RR |= (3<<14);	//
			HT_GPIOC->RR |= (1<<1);
			Delay(i); 
			HT_GPIOC->SRR |= (3<<14);	//
			HT_GPIOC->SRR |= (1<<1);
			Delay(12-i);
		}  
	}
	for(i=1;i<=12;i++)
	{
		j=0;
		while(++j<8)
		{   // tong time delay la 12*8 = 96ms
			HT_GPIOC->RR |= (3<<14);
			HT_GPIOC->RR |= (1<<1);
			Delay(12-i); 
			HT_GPIOC->SRR |= (3<<14);
			HT_GPIOC->SRR |= (1<<1);
			Delay(i);
		}  
	}
}
//
int main(void)
{
	// Enable Clock for PORT C(out); B,D(in)
	HT_CKCU->AHBCCR |= (7<<17);  // Clock for Port B,C,D

	// Configure PC14/LED0 , PC15/LED1 , PC1/LED2 as output pins
	HT_GPIOC->DIRCR |= (3<<14); //LED0, LED1
	HT_GPIOC->DIRCR |= (1<<1);	//LED2
	
	// Initial Value for PORT C(out)
	HT_GPIOC->DOUTR = 0xFFFF;
	// Reset the Corresponding pins		(SRR = led off, RR = led on)
	HT_GPIOC->RR |= (3<<14);  		//Reset LED0, LED1
	HT_GPIOC->RR |= (1<<1);		//Reset LED2

	SysTick_Config(48000); // Don't modify the Systick Calibration yet (divide 8)
	
	
	
	while(1)
	{
		//led dimmer
		led_dimmer();
	}

	//
}