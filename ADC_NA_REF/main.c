#include "ht32f5xxxx_01.h"              // Device header


int status1=0;
int status2=0;
int16_t status;
int value;
int count=0;
int count1=0;
int k=0;


//systick timer
volatile uint32_t msTicks = 0;
void Delay(uint32_t dlyTicks)
{
	uint32_t curTicks;
	curTicks = msTicks;
	while((msTicks-curTicks)<dlyTicks);
}
//
void SysTick_Handler(void)
{
	msTicks++;
}
//GPTM0
void GPTM0_IRQHandler(void)
{
	count1++;
	HT_GPTM0->INTSR &= ~(1<<8);						
}
//
void GPTM0_Init(void)
{
	HT_CKCU->APBCCR1 |= (1<<8);   		// GPTM0 clock
	HT_CKCU->APBPCSR0 |= (3<<20);   // PCLK = CK_AHB/8 = 6MHz
	HT_GPTM0->CNTCFR &= ~(1<<24); // counting up
	HT_GPTM0->CNTCFR &= ~(3<<16);// edge aligned mode
	HT_GPTM0->CNTCFR |= (1<<1);// Only overflow generates interrupt
	HT_GPTM0->CNTCFR &= ~(1<<0);
	HT_GPTM0->CNTR = 0;	//value start count
	HT_GPTM0->PSCR = 99;		//prescaler 999+1
	HT_GPTM0->CRR = 6000; //1ms interrupt once
	HT_GPTM0->DICTR |= (1<<8); 
	HT_GPTM0->CTR |= (1<<1);			
	HT_GPTM0->CTR |= (1<<0); 
	NVIC_EnableIRQ(GPTM0_IRQn);				
}
//
void EXTI0_1_IRQHandler(void) 
{		
	count++;
	if(count == 1)
	{
		status2 = 1;		
	}
	if(count == 2)
	{
		status2 = 2;
		count = 0;
	}
	HT_EXTI->EDGEFLGR |= (1<<1); 
}
//
void EXTI2_3_IRQHandler(void) 
{		
	status2 = 0;
	HT_EXTI->EDGEFLGR |= (1<<2); 
}
//
void Key1_interrupt_Int(void)
{
	HT_CKCU->AHBCCR |= (7<<17); // clock enable port b,c,d
	HT_CKCU->APBCCR0 |= (1<<15);// Interrupt clock enable
	HT_CKCU->APBCCR0 |= (1<<14);// AFIO clock enable
	HT_GPIOD->DIRCR &= ~(1<<1); //config input
	HT_GPIOD->INER |= (1<<1);//enable input
	HT_AFIO->ESSR[0] |= (3<<4); 
	HT_EXTI->CFGR1 |= (1<<29);	//interrupt button low
	HT_EXTI->CR |= (1<<1);  
	NVIC_EnableIRQ(EXTI0_1_IRQn);
}
//
void Key2_interrupt_Int(void)
{
	HT_CKCU->AHBCCR |= (7<<17); // clock enable port b,c,d
	HT_CKCU->APBCCR0 |= (1<<15);// Interrupt clock enable
	HT_CKCU->APBCCR0 |= (1<<14);// AFIO clock enable
	HT_GPIOD->DIRCR &= ~(1<<2); //config input
	HT_GPIOD->INER |= (1<<2);//enable input
	HT_AFIO->ESSR[0] |= (3<<8); 
	HT_EXTI->CFGR2 |= (3<<28);	//interrupt button low
	HT_EXTI->CR |= (1<<2);  
	NVIC_EnableIRQ(EXTI2_3_IRQn);
}
//
void ADC_Init(void)
{

	HT_CKCU->AHBCCR |= (1<<16); 		//Enable portA clock
	HT_CKCU->APBCCR1 |= (1<<24);	//Clock ADC Enable
	HT_CKCU->APBCCR0 |= (1<<14);		//Clock AFIO
	HT_CKCU->APBCFGR |= (3<<16); 	//ADC Clock PCLK = CK_AHB / 8
	HT_AFIO->GPACFGR[0] |= (2<<24); 
	//Configure ADC module
	HT_ADC->CR &= ~(3<<0); 		// one shot mode 
	HT_ADC->CR |= (7<<8); 		// One channel conversion
	HT_ADC->LST[0] |= (6<<0); 	// Analog ground? AVSS (VREF-) CH0
	HT_ADC->TCR |= (1<<0); 		// ADC conversion software trigger bit enable
	HT_ADC->IER |= (1<<24); 	// Enable overwrite interrupt
	//HT_ADC->IER |= (1<<2); 		// ADC cycle end of coversion interrupt is enable
	HT_ADC->CR |= (1<<7); 		// ADC Enable	
}
//Read ADC
int ADC_read()
{
	HT_ADC->TSR |= (1<<0); // Start conversion
	while((HT_ADC->DR[0] & (1<<31)));
	return ((HT_ADC->DR[0]) & ~(1<<31));
}
//
int main(void)
{
	//
	GPTM0_Init();	
	Key1_interrupt_Int();
	Key2_interrupt_Int();
	HT_CKCU->AHBCCR |= (7<<17); // clock enable port b,c,d
	HT_GPIOC->DIRCR |= (3<<14);   		// PC14 is output
	HT_GPIOC->DIRCR |= (1<<1);   		// PC14 is output
	HT_GPIOC->DOUTR = 0xffff;					// turn-off the LED
	SysTick_Config(48000); // Don't modify the Systick Calibration yet (divide 8)
	ADC_Init();
	//
	while(1)
	{
		
		if(count1 == 1)
		{
			for(k=0;k<2;k++)
			{
				value = value + ADC_read();
			}
			value = value / 3;
			count1 = 0;
		}
		if(status2 == 0)
		{
			value = ADC_read();	
			if(value < 2000)
			{
				status1 = 1;
			}
			if(value >= 2000 & value <= 3000)		
			{
				status1 = 2;
			}	
			if(status1 == 1)
			{
				HT_GPIOC->RR |= (1<<14);
			}
  
		}
		if(status1 == 2 & status2 != 2)
		{
			HT_GPIOC->RR |= (3<<14);
			HT_GPIOC->RR |= (1<<1);
			Delay(500);
			HT_GPIOC->SRR |= (3<<14);
			HT_GPIOC->SRR |= (1<<1);
			Delay(500);
		}
		if(status2 == 1) HT_GPIOC->SRR |= (1<<14);
		if(status2 == 2) 
		{
			 HT_GPIOC->SRR |= (3<<14);
			 HT_GPIOC->SRR |= (1<<14);
		}			
	}
}