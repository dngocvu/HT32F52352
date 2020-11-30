#include "ht32f5xxxx_01.h"             
volatile uint32_t msTicks = 0;
volatile uint32_t Time = 0;
volatile uint32_t kc = 0;
int tt1=1;
int i=0;
int a,b,c;
void Delay(uint32_t dlyTicks)
{
      uint32_t curTicks;
      curTicks = msTicks;
      while ((msTicks - curTicks) < dlyTicks) ;
}
void GPTM0_IRQHandler(void)
{
	msTicks++;
	HT_GPTM0->INTSR &= ~(1<<8);						
}
void EXTI4_15_IRQHandler(void) 
{
	tt1=12;
	HT_EXTI->EDGEFLGR |= (1<<12); 
}
void EXTI2_3_IRQHandler(void) 
{
	if((HT_EXTI->EDGESR&(1<<3))==8)
		{
			HT_GPTM1->CNTR = 0;
	    HT_GPTM1->CTR |= (1<<0);
		}
	if((HT_EXTI->EDGESR&(1<<3))==0)
		{
			HT_GPTM1->CTR &=~ (1<<0);
	    kc=((HT_GPTM1->CNTR)*1)/58.82;
		}		
	HT_EXTI->EDGEFLGR |= (1<<3); 
}
void send_max7219(uint8_t addr, uint8_t data)
{
	uint16_t Data_to_send;	
	HT_SPI0->CR0 |=(1<<4); 
	HT_SPI0->DR =(addr<< 8)|data;
  HT_SPI0->DR =Data_to_send;
	while (((HT_SPI0->SR)&(1<<1))==0); 
	HT_SPI0->CR0 &=~(1<<4); 
}
void Configure_MAX7219(void)
{
send_max7219(0x09, 0xff);  //REG_DECODE_MODE 
send_max7219(0x0A, 0x07);  //REG_INTENSITY 	
send_max7219(0x0B, 0x02);  //REG_SCAN_LIMIT 	
send_max7219(0x0C, 0x01);  //REG_SHUTDOWN
send_max7219(0x0F, 0x00);  //REG_DISPLAY_TEST 
}
void Showled (void) // Hien thi MSSV
{
    send_max7219(8, 0);
		send_max7219(7, 0);
		send_max7219(6, 0);
		send_max7219(5, 0);
		send_max7219(4, 0);
		send_max7219(3, 1);
		send_max7219(2, 1);
		send_max7219(1, 1);
}
void MAX7219_Clear(void)
{
  unsigned char i;
  for(i=8;i>0;i--) 
	send_max7219(i,0x00);
}
int main(void)
{
	// Enable clock for GPIO, GPTM0
	HT_CKCU->AHBCCR |= (7<<17); 
	HT_CKCU->APBCCR0 |= (3<<14);
	HT_CKCU->AHBCCR |= (1<<18);   		// PORTC clock
	HT_CKCU->AHBCCR |= (1<<17);   		// PORTbB clock
	HT_CKCU->APBCCR1 |= (1<<8);   		// GPTM0 clock
	HT_CKCU->APBCCR1 |= (1<<9);   		// GPTM0 clock
	HT_GPIOB->DIRCR &=~ (1<<3);				// PB3 is input
	HT_GPIOB->INER|=(1<<3);           //enable input
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
	HT_GPIOC->DOUTR = 0xFFFF;					// turn-off the LEDs
	HT_GPIOB->DIRCR |= (1<<4);   		// PB4 is output
	HT_GPIOB->DIRCR &=~ (1<<3);				// PB3 is inputput
	HT_GPIOB->INER|=(1<<3);
	HT_GPIOB->DOUTR = 0x0000;					// turn-off the LEDs
	// Select GPTM0 APB prescaler
	HT_CKCU->APBPCSR0 |= (3<<22); 		// PCLK = CK_AHB/8 = 6MHz
	HT_CKCU->APBPCSR0 |= (3<<20); 
	// Configure GPTM timer: counting up, reload value, enable timer, interrupt
	HT_GPTM1->CNTCFR &= ~(1<<24); 	// counting up
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
	HT_GPTM0->PSCR = 0;		
	HT_GPTM1->CRR = 60000;						
	HT_GPTM0->CRR = 6;				
	HT_GPTM1->DICTR |= (1<<8); 				
	HT_GPTM0->DICTR |= (1<<8); 
	HT_GPTM1->CTR |= (1<<1);					
	HT_GPTM0->CTR |= (1<<1);			
  HT_GPTM0->CTR |= (1<<0); 
	NVIC_EnableIRQ(GPTM0_IRQn);	
	
  HT_CKCU->APBCCR0 |= (1<<14); //AFIO
	HT_CKCU->APBCCR0 |= (1<<4) ;//SPI0
	HT_CKCU->APBPCSR0 |= (3<<4); // SPI0, PCLK = AHB/8 = 6 MHz
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
	Configure_MAX7219();
	MAX7219_Clear();
  //SysTick_Config(48000);		
while(1)
{
		HT_GPIOB->SRR |= (1 << 4);// Trigger =1
		Delay(10);
		HT_GPIOB->RR |= (1 << 4);//Trigger=0
	if(kc <=5)
		{
			HT_GPIOC->RR |= (1 << 14);		
		}
	else
		{
			HT_GPIOC->SRR |= (1 << 14);
		}
			for( i=1;i<=1000;i++)
			{
				kc=(((HT_GPTM1->CNTR)*1)/58.82)+kc;	
			}
		kc=kc/1000;
		a=kc/100;
		kc=kc%100;
		b=kc/10;
		c=kc%10;		
	  Showled ();
}			
      }