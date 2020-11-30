#include "ht32f5xxxx_01.h"              // Device header


//Trigger-PB0
//Echo-PB4

uint32_t time, distance;

void Delay(uint16_t microsecond)
{
	HT_BFTM1->SR = 0; //Reset BFTM1
	HT_BFTM1->CR |= (1<<2); //Enable BFTM1 
	while(HT_BFTM1->CNTR < microsecond); //Wait until BFTM1 reaches the disired delay 
	HT_BFTM1->CR &=~ (1<<2); //Disable BFTM1
}

void EXTI4_15_IRQHandler(void)
{
	HT_BFTM0->SR = 0; //Reset BFTM0
	HT_BFTM0->CR &=~ (1<<2); //Disable BFTM0
	time = HT_BFTM0->CNTR; //Count timer BFTM0
}

void Init(void)
{
	//Configure Clock for GPIO, BFTM, EXIO
	HT_CKCU->AHBCCR |= (1<<18); //Clock for PORTC
	HT_CKCU->APBCCR1 |= (1<<16); //Clock for BFTM
	HT_CKCU->APBCCR0 |= (1<<15); //Clock for EXTI
	
	//Configure PB0/Trigger, PC14/LED0, PC15/LED1, PC1/LED2 is output pin
	HT_GPIOC->DIRCR |= (3<<14); //LED0, LED1
	HT_GPIOC->DIRCR |= (1<<1); //LED2
	HT_GPIOB->DIRCR |= (1<<0); //Trigger
	
	//Initial value for PORTC
	HT_GPIOC->DOUTR = 0xFFFF;
	
	//Configure PB4/Echo is input pin
	HT_GPIOB->DIRCR &=~ (1<<4); //Echo
	
	//Assign EXTI function to GPIO pin
	HT_AFIO->ESSR[0] |= (1<<16); //PB4-Echo-EXTI4
	
	//Configure egde detection, debounce
	HT_EXTI->CFGR1 |= (2<<28); //Negative egde Trigger for EXTI4
	
	//Enable EXTI interupt pin
	HT_EXTI->CR |= (1<<4); //Enable for EXTI4
	
	//Enable EXTI Interrupt
	NVIC_EnableIRQ(EXTI4_15_IRQn);
		
	//Init for BFTM
	HT_BFTM0->CR &=~ (1<<2); //Disable BFTM0
	HT_BFTM0->CNTR = 0; //Reset counter
	HT_CKCU->APBPCSR0 |= (3<<12); //PCLK = CK_AHB/8 = 1MHz

	//Reset the corecsponding pins
	HT_GPIOC->RR |= (1<<14); //Reset LED0
	HT_GPIOC->RR |= (1<<15); //Reset LED1
	HT_GPIOC->RR |= (1<<1); //Reset LED2
	
}
int main()
{
	Init();
	while(1)
	{
		HT_GPIOB->DOUTR |= (1<<0); //Trigger pin to high
		Delay(10);
		HT_GPIOB->DOUTR &=~ (1<<0); //Trigger pin to low
		
		while(((HT_GPIOB->DINR & (1<<4)) == (1<<4))) //Wait for interupt pin to 1 level
		{
			HT_BFTM0->CR |= (1<<2); //Enable Timer 
		}
		distance = (0.0343 * time)/2;
		if(distance < 20) HT_GPIOC->DOUTR |= (1<<14); //LED0 bright
		if(distance > 20 && distance < 50) HT_GPIOC->DOUTR |= (1<<15); //LED1 bright
		if(distance > 50) HT_GPIOC->DOUTR |= (1<<1); //LED2 bright
	}
}

