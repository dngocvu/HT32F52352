#include "ARM/INC/Holtek/HT32F5xxxx/ht32f5xxxx_01.h" // Device header

volatile uint32_t msTicks = 0; 
uint16_t tam = 0;

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


void EXTI0_1_IRQHandler(void) // Handle for EXTI_1, SECTION 9/PAGE 177
		{		
			tam=1;
			HT_EXTI->EDGEFLGR |=(1<<1); // Clear interrupt flag of EXTI1
		}

void EXTI2_3_IRQHandler(void) // Handle for EXTI_2
		{
			tam=2;
			HT_EXTI->EDGEFLGR |=(1<<2); // Clear interrupt flag of EXTI2
		}	
	
void EXTI4_15_IRQHandler(void) //  Handle for EXTI12
		{
			tam=12;
			HT_EXTI->EDGEFLGR |= (1<<12); // Clear interrupt flag of EXTI12
		}
int main(void)
{
	// Enable Clock for PORT C(out); B,D(in) SECTION 6/ PAGE 102
	HT_CKCU->AHBCCR |= (1<<17);  // Clock for Port B
	HT_CKCU->AHBCCR |= (1<<18);  // Clock for Port C
	HT_CKCU->AHBCCR |= (1<<19);  // Clock for Port D
	//Enable Clock for EXTI     SENTION 6/ PAGE 105
	HT_CKCU->APBCCR0 |= (1<<15);
	//Enable Clock for AFIO		SENTION 6/ PAGE 105
	HT_CKCU->APBCCR0 |= (1<<14);
	
	 // Configure PC14/LED0 , PC15/LED1 , PC1/LED2 as output pins
	HT_GPIOC->DIRCR |= (1<<14); //LED0
	HT_GPIOC->DIRCR |= (1<<15);	//LED1
	HT_GPIOC->DIRCR |= (1<<1);	//LED2
	
	// Set up AF1 (GPIO) for PB12 (overview the Pin Assignment_Datasheet/ AF0(default) is not GPIO)
	HT_AFIO->GPBCFGR[1] |= (1<<16);
	
	// Configure PD1/Button1 , PD2/Button2, PB12/WakeUp as input pins
	HT_GPIOD->DIRCR &=~(1<<1);  // Button1
	HT_GPIOD->DIRCR &=~(1<<2);  // Button2
	HT_GPIOB->DIRCR &=~(1<<12); // WakeUp
	HT_GPIOD->INER = 0xFFFF;
	HT_GPIOB->INER = 0xFFFF;
	
	
	//Assign EXTI function to GPIO pins
	HT_AFIO->ESSR[0] |= (3<<4);  // PD1-Button1-EXTI1   3 <=> 0011 (port D)
	HT_AFIO->ESSR[0] |= (3<<8);	 // PD2-Button2-EXTI2
	HT_AFIO->ESSR[1] |= (1<<16); // PB12-WakeUp-EXTI12  1 <=> 0001 (port B)
 
	//Configure edge detection, de-bounce
	HT_EXTI->CFGR1 |= (3<<28);	// Positive edge Trigger for EXT1
	HT_EXTI->CFGR2 |= (3<<28);   // Positive edge Trigger for EXT2
	HT_EXTI->CFGR12 |= (3<<28); // Positive edge Trigger for EXT12
	
	// Enable EXTI interrupt pins
	HT_EXTI->CR |= (1<<1);  // Enable for EXTI1
	HT_EXTI->CR |= (1<<2);  // Enable for EXTI2
	HT_EXTI->CR |= (1<<12);  // Enable for EXTI12
	// Enable EXTI interrupts
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	NVIC_EnableIRQ(EXTI2_3_IRQn);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
	// Initial Value for PORT C(out)
	HT_GPIOC->DOUTR = 0xFFFF;
	// Reset the Corresponding pins
	HT_GPIOC->RR |= (1<<14);  //Reset LED0
	HT_GPIOC->RR |= (1<<15);  //Reset LED1
	HT_GPIOC->RR |= (1<<1);		//Reset LED2


SysTick_Config(48000); // Don't modify the Systick Calibration yet (divide 8) 8MHz * (PLL = 6) = 48000Hz(per clock pulse )
		while(1)
		{
			if(tam==0) 
			{
				HT_GPIOC->DOUTR = ~HT_GPIOC->DOUTR;
				Delay(250);
			}
			if(tam==12) 
			{
				HT_GPIOC->SRR |= (1<<14);
				Delay(200);
				HT_GPIOC->RR |= (1<<14);
				Delay(200);
			}
		 if(tam==1)
			{
				HT_GPIOC->SRR |= (1<<15);
				Delay(200);
				HT_GPIOC->RR |= (1<<15);
				Delay(200);
			}
		 if(tam==2)
			{
				HT_GPIOC->SRR |= (1<<1);
				Delay(200);
				HT_GPIOC->RR |= (1<<1);
				Delay(200);
			}
		}

}