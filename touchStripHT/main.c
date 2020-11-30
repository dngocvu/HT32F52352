#include "ht32f5xxxx_01.h"              // Device header

uint16_t status;
volatile uint32_t msTicks = 0;

//
void Delay(uint32_t dlyTicks)
{
	uint32_t curTicks;
	
	curTicks = msTicks;
	while((msTicks - curTicks) < dlyTicks);
}
//
void BFTM0_IRQHandler(void)
{
	msTicks++;
	HT_BFTM0->SR = 0;			// clear the BFTM interrupt flag
}
//
void I2C_int(void)
{                       
	HT_I2C1->SHPGR = 24;   
	HT_I2C1->SLPGR = 24;   
	HT_I2C1->CR |=(1<<3);   // Enable I2C1	       
}
//
void Write_Reg(unsigned char address, int data)
{
	while((HT_I2C1 -> SR & (1<<1))); 
	// check BUSY bit 
	while((HT_I2C1 -> SR & (1<<19)));			
	// Set bit TAR[10] = 0 - > Write ; TAR[9:0] = MPU address
	HT_I2C1 -> TAR &=~ (1<<10);          
	HT_I2C1 -> TAR |= (0x50);            //write slave adress in tar
	while(!(HT_I2C1 -> SR & (1<<0)));
	while(!(HT_I2C1 -> SR & (1<<2)));	  // wait untill the slave send ACK
	while(!(HT_I2C1 -> SR & (1<<17)));    //wait Data register I2CDR not empty
	while(!(HT_I2C1 -> SR & (1<<19))); 
	while(!(HT_I2C1 -> SR & (1<<21))); 
	HT_I2C1 -> DR = address;                 //data adress second
	while(!(HT_I2C1 -> SR & (1<<17)));    	// wait Data register I2CDR not empty
	HT_GPIOC -> SRR |= (3<<14);				// led check
	HT_I2C1 -> DR = data;              	// write second
	while(!(HT_I2C1 -> SR & (1<<17)));
	HT_I2C1 -> CR |= (1<<1);              	// stop bit
}
//
uint16_t Read_Reg(unsigned char address)
	{
		uint16_t data;
		// check BUSY bit 
		while((HT_I2C1->SR & (1<<19)));		//check if i2c busy			
		// Set bit TAR[10] = 0 - > Write ; TAR[9:0] = BS8112-3 address
		HT_I2C1->TAR &=~ (1<<10);          	//set mode write
		HT_I2C1->TAR |= (0x50);            	//write slave adress in tar
		while(!(HT_I2C1->SR & (1<<2)));		// wait until ADRS = 1
		while(!(HT_I2C1->SR & (1<<17)));	// check TXDE 		
		HT_I2C1->DR = address;              //data adress second
		while(!(HT_I2C1->SR & (1<<17)));	// check TXDE 
		// Set bit TAR[10] = 1 -> Read ; TAR[9:0] = MPU address
		HT_I2C1->TAR |= (1<<10);
		HT_I2C1->TAR |= (0x50);
		// wait untill the slave send ACK
		while(!(HT_I2C1->SR & (1<<2)));    
		while(!(HT_I2C1->SR & (1<<19))); 
		// Send NACK
		HT_I2C1->CR &=~ (1<<0); 
		while(!(HT_I2C1->SR & (1<<16)));    // check buffer full
		data = HT_I2C1->DR;               	// read second
		// Stop bit
		HT_I2C1->CR |= (1<<1);
		
		return data;	
}
//	
void read_touchkey(void)
{
	status = Read_Reg(0x08);
}
//
int main(void)
{
		// Enable clock for PORT A, B, C
		HT_CKCU->AHBCCR |= (7<<17); 
		// Enable clock for AFIO
		HT_CKCU->APBCCR0 |= (3<<14);
		// Enable clock for I2C
		HT_CKCU->APBCCR0 |= (1<<1);
		// Enable Clock BFTM0
		HT_CKCU->APBCCR1 |= (1<<16);  	
		// Configure PC14/LED0 , PC15/LED1 , PC1/LED2 as output pins
		HT_GPIOC->DIRCR |= (1<<14);
		HT_GPIOC->DIRCR |= (1<<15);
		HT_GPIOC->DIRCR |= (1<<1);	
		// Configure Value for PC14, PC15, PC1
		HT_GPIOC->SRR |= (1<<14);
		HT_GPIOC->SRR |= (1<<15);
		HT_GPIOC->SRR |= (1<<1);
		// I2C
		HT_AFIO->GPACFGR[0] |= (7<<0);   //Touch_SCL(PA0)
		HT_AFIO->GPACFGR[0] |= (7<<4);   //Touch_SDA(PA1)
		HT_CKCU->APBCCR0 |= (3<<14);     //Enable CLK I2C1
		HT_CKCU->APBCCR0 |= (1<<14);     //Enable CLK I2C1 AFIO
		HT_CKCU->APBPCSR0 |= (3<<2);     //I2C1, PCLK = AHB/2 = 24 MHz
		I2C_int();
		// Select BFTM APB prescaler
		HT_CKCU->APBPCSR0 |= (3<<12); // PCLK = CK_AHB/8 = 6MHz
		// Configure BFTM timer
		HT_BFTM0->CNTR = 0;						// Reset Counter
		HT_BFTM0->CMP = 120000;      		// 1ms / cycle timer
		HT_BFTM0->CR = 5;							// Enable Timer0, Enable Interrupt, Repetitive Mode
		//
		NVIC_EnableIRQ(BFTM0_IRQn);
	//
	while(1)
	{
		read_touchkey();
		if (status == 0)
		{
			HT_GPIOC->RR |= (1<<14);	
			HT_GPIOC->RR |= (1<<15);
			HT_GPIOC->RR |= (1<<1);
		}
		if ( 0 < status && status < 10)
		{
			HT_GPIOC->RR |= (1<<14);
			HT_GPIOC->SRR |= (1<<15);
			HT_GPIOC->SRR |= (1<<1);
		}
		else if (20 < status && status < 60)
		{
			HT_GPIOC->RR |= (1<<15);
			HT_GPIOC->SRR |= (1<<14);
			HT_GPIOC->SRR |= (1<<1);
		}
		else if (70 < status && status <= 80)
		{
			HT_GPIOC->RR |= (1<<1);
			HT_GPIOC->SRR |= (1<<15);
			HT_GPIOC->SRR |= (1<<14);
		}
	}
}