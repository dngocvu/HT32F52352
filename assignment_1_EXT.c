#include "ht32f5xxxx_01.h"              // Device header


volatile uint32_t msTicks = 0;
uint16_t count = 0, i;

void Delay(uint32_t dlyTicks)
{
	uint32_t curTicks;
	
	curTicks = msTicks;
	while((msTicks-curTicks)<dlyTicks);
}


void SysTick_Handler(void)
{
	// Increment counter necessary in Delay()
	msTicks++;
}


void EXTI0_1_IRQHandler(void) // Handle for EXTI_1
		{	
			if((HT_EXTI->EDGESR & (1<<1)) == (1<<1))
			{
				count=0;
			}
			else
			{
				count=1;
			}
			HT_EXTI->EDGEFLGR |=(1<<1); // Clear interrupt flag of EXTI1
		}

void EXTI2_3_IRQHandler(void) // Handle for EXTI_2
		{
			count=2;
			HT_EXTI->EDGEFLGR |=(1<<2); // Clear interrupt flag of EXTI2
		}	
	
void EXTI4_15_IRQHandler(void) //  Handle for EXTI12
		{
			count=12;
			HT_EXTI->EDGEFLGR |= (1<<12); // Clear interrupt flag of EXTI12
		}
int main(void)
{
	// Enable Clock for PORT C(out); B,D(in)
	HT_CKCU->AHBCCR |= (7<<17);  // Clock for Port B,C,D
	
	//Enable Clock for EXTI
	HT_CKCU->APBCCR0 |= (1<<15);
	
	//Enable Clock for AFIO
	HT_CKCU->APBCCR0 |= (1<<14);
	
	// Configure PC14/LED0 , PC15/LED1 , PC1/LED2 as output pins
	HT_GPIOC->DIRCR |= (3<<14); //LED0, LED1
	HT_GPIOC->DIRCR |= (1<<1);	//LED2
	
	// Set up AF1 (GPIO) for PB12 (overview the Pin Assignment_Datasheet/ AF0(default) is not GPIO)
	HT_AFIO->GPBCFGR[1] |= (1<<16);
	
	// Configure PD1/Button1 , PD2/Button2, PB12/WakeUp as input pins
	HT_GPIOD->DIRCR &=~(3<<1);  // Button1, Button2
	HT_GPIOB->DIRCR &=~(1<<12); // WakeUp
	HT_GPIOD->INER = 0xFFFF;
	HT_GPIOB->INER = 0xFFFF;
	
	
	//Assign EXTI function to GPIO pins
	HT_AFIO->ESSR[0] |= (3<<4);  // PD1-Button1-EXTI1
	HT_AFIO->ESSR[0] |= (3<<8);	 // PD2-Button2-EXTI2
	HT_AFIO->ESSR[1] |= (1<<16); // PB12-WakeUp-EXTI12
 
	//Configure edge detection, de-bounce
	HT_EXTI->CFGR1 |= (4<<28);	// Positive edge Trigger for EXT1
	HT_EXTI->CFGR2 |= (3<<28);   // Positive edge Trigger for EXT2
	HT_EXTI->CFGR12 |= (3<<28); // Positive edge Trigger for EXT12
	
	// Enable EXTI interrupt pins
	HT_EXTI->CR |= (3<<1);  // Enable for EXTI1, EXTI2
	HT_EXTI->CR |= (1<<12);  // Enable for EXTI12
	// Enable EXTI interrupts
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	NVIC_EnableIRQ(EXTI2_3_IRQn);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
	
	//Configure the External Interupt priority
	NVIC_SetPriority(EXTI12_IRQn, 0);
	NVIC_SetPriority(EXTI1_IRQn, 1);
	NVIC_SetPriority(EXTI2_IRQn, 1);
	
	// Initial Value for PORT C(out)
	HT_GPIOC->DOUTR = 0xFFFF;
	// Reset the Corresponding pins
	HT_GPIOC->RR |= (3<<14);  //Reset LED0, LED1
	HT_GPIOC->RR |= (1<<1);		//Reset LED2

	SysTick_Config(48000); // Don't modify the Systick Calibration yet (divide 8)
	
		while(1)
		{
			//Press Wakeup
			if(count==12) 
			{
				HT_GPIOC->DOUTR &=~ (3<<14); //LED0, LED1 up to 1-level
				HT_GPIOC->DOUTR &=~ (1<<1); //LED2 up to 1-level
			}
			
			//Press Button1
			if(count==0) 
			{
				HT_GPIOC->DOUTR |= (3<<14); //LED0, LED1 down to 0-level
				HT_GPIOC->DOUTR |= (1<<1); //LED2 down to 0-level 
			}
		  if(count==1)
			{
				HT_GPIOC->SRR |= (3<<14);
				HT_GPIOC->SRR |= (1<<1);
				Delay(200);
				HT_GPIOC->RR |= (3<<14);
				HT_GPIOC->RR |= (1<<1);
				Delay(200);
			}
			
			//Press Button2
			if(count==2)
			{
				for(i=0;i<5;i++)
				{
					HT_GPIOC->SRR |= (3<<14);
					HT_GPIOC->SRR |= (1<<1);
					Delay(300);
					HT_GPIOC->RR |= (3<<14);
					HT_GPIOC->RR |= (1<<1);
					Delay(300);
					if(count != 2)
					{		
						break;	
					}						
				}
				if(i==5) count=0;
			}
		}

}