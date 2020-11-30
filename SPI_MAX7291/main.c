#include "ht32f5xxxx_01.h"              // Device header


volatile uint32_t msTicks = 0; 
uint16_t sum=0, j=0;
unsigned char  cnt = 0,SEG7_code[19]={0x7e,0x30,0x6d,0x79,0x33,0x5b,0x5f,0x70,0x7f,0x7b};

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
void SendAddrData (uint16_t addr,uint16_t data)
{
	
	HT_SPI0->CR0 |= (1<<4);
	HT_SPI0->DR &= 0x0000;
	sum = (addr<<8) | data;
	HT_SPI0->DR |= sum;
	Delay(1);
	HT_SPI0->CR0 &= ~(1<<4);
	Delay(1);
}
/*void Init (void)
{
    SendAddrData (0x0c,0x01); //normal operation
    SendAddrData (0x0a,0x0D); //intensity
    SendAddrData (0x0b,0x07); //all digits on
    SendAddrData (0x09,0x00); //decoding
    SendAddrData (0x0f,0x00); //display test off
}*/
void Clear(void)
{
    unsigned char i;
    for(i=8;i>0;i--) SendAddrData(i,0x00);
	
}

void Student_code(uint16_t i)
{
	//Student_code_Duy = 16146262
	if(i==0){
		SendAddrData(0x08,SEG7_code[1]);
		
		SendAddrData(0x07,SEG7_code[6]);
			
		SendAddrData(0x06,SEG7_code[1]);
			
		SendAddrData(0x05,SEG7_code[4]);
			
		SendAddrData(0x04,SEG7_code[6]);
			
		SendAddrData(0x03,SEG7_code[2]);
			
		SendAddrData(0x02,SEG7_code[6]);
			
		SendAddrData(0x01,SEG7_code[2]);
	}
	
	//Student_code_Nhan = 16146426
	if(i==1){
		SendAddrData(0x08,SEG7_code[1]);
		
		SendAddrData(0x07,SEG7_code[6]);
			
		SendAddrData(0x06,SEG7_code[1]);
			
		SendAddrData(0x05,SEG7_code[4]);
			
		SendAddrData(0x04,SEG7_code[6]);
			
		SendAddrData(0x03,SEG7_code[4]);
			
		SendAddrData(0x02,SEG7_code[2]);
			
		SendAddrData(0x01,SEG7_code[6]);
	}
	
	//Student_code_Vu = 16146578
	if(i==2){
		SendAddrData(0x08,SEG7_code[1]);
		
		SendAddrData(0x07,SEG7_code[6]);
			
		SendAddrData(0x06,SEG7_code[1]);
			
		SendAddrData(0x05,SEG7_code[4]);
			
		SendAddrData(0x04,SEG7_code[6]);
			
		SendAddrData(0x03,SEG7_code[5]);
			
		SendAddrData(0x02,SEG7_code[7]);
			
		SendAddrData(0x01,SEG7_code[8]);
	}
	
	//Student_code_Truong = 16146548
	if(i==3){
		SendAddrData(0x08,SEG7_code[1]);
		
		SendAddrData(0x07,SEG7_code[6]);
			
		SendAddrData(0x06,SEG7_code[1]);
			
		SendAddrData(0x05,SEG7_code[4]);
			
		SendAddrData(0x04,SEG7_code[6]);
			
		SendAddrData(0x03,SEG7_code[5]);
			
		SendAddrData(0x02,SEG7_code[4]);
			
		SendAddrData(0x01,SEG7_code[8]);
	}
}

int main (){
	//Enable Clock for AFIO, SPI, GPIO
	HT_CKCU->APBCCR0 |= (1<<14);
	HT_CKCU->APBCCR0 |= (1<<4);
	HT_CKCU->AHBCCR |= (1<<16);
	
	//Assign AF5(SPI) to PA4, PA5, PA7
	HT_AFIO->GPACFGR[0] |= (5<<16); //PA4
	HT_AFIO->GPACFGR[0] |= (5<<20); //PA5
	HT_AFIO->GPACFGR[0] |= (5<<28); //PA7
	
	//Select SPI0 APB prescaler
	HT_CKCU->APBPCSR0 |= (2<<4); //PCLK = CK_AHB/4 = 12MHz
	
	//Configure operation mode of SPI
	HT_SPI0->CR1 |= (1<<8); //Transfer format with CPOL=0, CPHA=0
	HT_SPI0->CR1 &=~ (1<<13); //SEL signal is controlled by software
	HT_SPI0->CR1 &=~ (1<<11); //SEL signal is active Low
	HT_SPI0->CR0 |= (1<<3); //Set the SEL signal to the output mode for slave select
	HT_SPI0->CR1 &=~ (1<<12); //MSB transmitted first
	HT_SPI0->CR1 |= (1<<14); //Master mode
	HT_SPI0->CPR |= 0; //SCK = PCLK/(2*(0+1)) = 6MHz
	
	HT_SPI0->CR0 |= 1; //SPI interface is enabled
	SysTick_Config(48000); // Don't modify the Systick Calibration yet (divide 8)
	
	
	//Init();
	Clear();
	//Student_code();
	
	
	while (1)
	{
		j=(j+1)%4;
		Student_code(j);
		for(cnt=0;cnt<14;cnt++)
		{
			SendAddrData(0x0a,cnt);
			Delay(100);
		}
		for(cnt=14;cnt>0;cnt--)
		{
			SendAddrData(0x0a,cnt);
			Delay(100);
		}
	}
}