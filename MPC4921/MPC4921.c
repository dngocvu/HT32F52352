#include "ht32f5xxxx_01.h"          
#include <stdint.h>
#include <math.h>

//   
volatile uint32_t msTicks = 0;
static uint16_t Volt[21] ={0,512,1024,1536,2048,2560,3072,3584,4096,4096,4096,4096,4096,3584,3072,2560,2048,1536,1024,512,0};
int i=0;

//
void send_max7219(uint16_t data)
{
	uint16_t Data_to_send;	
	HT_SPI0->CR0 |=(1<<4); 
	Data_to_send=(((data>>8)|0x30)<<8)|(data&0x00ff);
	HT_SPI0->DR =Data_to_send;
	while (((HT_SPI0->SR)&(1<<1))==0); 
	HT_SPI0->CR0 &=~(1<<4); 
}
//
void GPTM0_IRQHandler(void)
{
	if(i>=22)
		{
			i=0;
		}
	send_max7219(Volt[i++]);
	HT_GPTM0->INTSR &= ~(1<<8);						
}
//
int main(void)
{
	// Enable clock for GPIO, GPTM0
	HT_CKCU->AHBCCR |= (7<<17); 
	HT_CKCU->APBCCR0 |= (3<<14);
	HT_CKCU->APBCCR1 |= (1<<8);   		// GPTM0 clock
	HT_CKCU->APBCCR1 |= (1<<9);   		// GPTM0 clock
	HT_GPIOB->DIRCR &=~ (1<<3);			// PB3 is input
	HT_GPIOB->INER|=(1<<3);           	//enable input
	HT_AFIO->ESSR[0] |= (1<<12);
	HT_EXTI->CFGR3 |=(7<<28);
	HT_EXTI->CR |= (1<<3); 
	NVIC_EnableIRQ(EXTI2_3_IRQn); 
	HT_AFIO->GPBCFGR[1] |= (1<<16);
	HT_GPIOB->DIRCR &=~(1<<12); 
	HT_GPIOB->INER |=(1<<12);
	HT_AFIO->ESSR[1] |= (1<<16);
	HT_EXTI->CFGR12 |=(1<<28);				
	HT_EXTI->CR |= (1<<12);  
	NVIC_EnableIRQ(EXTI4_15_IRQn);
	// Configure LED pins as output
	HT_GPIOC->DIRCR |= (1<<14);   		// PC14 is output
	HT_GPIOC->DOUTR = 0xFFFF;			// turn-off the LEDs
	HT_GPIOB->DIRCR |= (1<<4);   		// PB4 is output
	HT_GPIOB->DIRCR &=~ (1<<3);			// PB3 is inputput
	HT_GPIOB->INER|=(1<<3);
	HT_GPIOB->DOUTR = 0x0000;			// turn-off the LEDs
	// Select GPTM0 APB prescaler
	HT_CKCU->APBPCSR0 |= (3<<22); 		// PCLK = CK_AHB/8 = 6MHz
	HT_CKCU->APBPCSR0 |= (3<<20); 
	// Configure GPTM timer: counting up, reload value, enable timer, interrupt
	HT_GPTM1->CNTCFR &= ~(1<<24); 		// counting up
	HT_GPTM0->CNTCFR &= ~(1<<24);
	HT_GPTM1->CNTCFR &= ~(3<<16);		// edge aligned mode
	HT_GPTM0->CNTCFR &= ~(3<<16);	
	HT_GPTM1->CNTCFR |= (1<<1); 		// Only overflow generates interrupt
	HT_GPTM0->CNTCFR |= (1<<1);
	HT_GPTM1->CNTCFR &= ~(1<<0);		
	HT_GPTM0->CNTCFR &= ~(1<<0);
	HT_GPTM1->CNTR = 0;								
	HT_GPTM0->CNTR = 0;	
	HT_GPTM1->PSCR = 5;							
	HT_GPTM0->PSCR = 99;		
	HT_GPTM1->CRR = 60000;						
	HT_GPTM0->CRR = 60000;				
	HT_GPTM1->DICTR |= (1<<8); 				
	HT_GPTM0->DICTR |= (1<<8); 
	HT_GPTM1->CTR |= (1<<1);					
	HT_GPTM0->CTR |= (1<<1);			
	HT_GPTM0->CTR |= (1<<0); 
	NVIC_EnableIRQ(GPTM0_IRQn);	
	
	HT_CKCU->APBCCR0 |= (1<<14); //AFIO
	HT_CKCU->APBCCR0 |= (1<<4) ;//SPI0
	HT_CKCU->APBPCSR0 |= (1<<4); // SPI0, PCLK = AHB/8 = 6 MHz
	HT_AFIO->GPACFGR[0] |= (5<<16); //SPI0_SCK_PA4
	HT_AFIO->GPACFGR[0] |= (5<<20); //SPI0_MOSI_PA5
	HT_AFIO->GPACFGR[0] |= (5<<28); //SPI0_SEL_PA7
	HT_SPI0->CR1 |=(1<<14);// Mode Master
	HT_SPI0->CR1 &=~(1<<13);  // Slave Select Mode  SEL signal is controlled by software
	HT_SPI0->CR1 |=(1<<8); //Tranfer format:CPOL=0,CPHA=0
	HT_SPI0->CR1 &=~(1<<11);// Selporality
	HT_SPI0->CR1 &=~(1<<12);//MSB Out Firt
	HT_SPI0->CR0 |=(1<<3);  // Enable Select Slave
	HT_SPI0->CPR = 0;//SPI clock prescaler
	HT_SPI0->CR0 |=(1<<0);//Enable Spi	
	while(1)
	{

	}			
}