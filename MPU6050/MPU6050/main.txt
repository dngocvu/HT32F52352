#include "ht32f5xxxx_01.h"          // Device header
#include <math.h>
#include <stdint.h>

//void init_Clock(void)
//{
//	///////////////////////////////////////////
//	
//	////////////////////////////////////////////////////////////
//}
////////////////////////////////////////////////////////
// Specifies the i2c addressing mode, Stop bit, ACK bit, enable I2C in I2CCR register

// Assign address to I2C devices in I2CADDR and I2CTAR registers

// Specifies the I2C clock period in I2CSHPGR/I2CSLPGR registers

// Enable I2C interrupts (optional) in I2CIER register
/////////////////////////////////////////////////////////
void I2C_init(void)
{   
		//set clock's speed
		HT_I2C1 -> SLPGR = 24;
		HT_I2C1 -> SHPGR = 24;
		// 7 bit address
		HT_I2C1 -> CR &= ~(1<<7);
		// enable I2C
		HT_I2C1 -> CR |= (1<<3);         
}
void writeReg(unsigned char address, int data)
{
		while((HT_I2C1 -> SR & (1<<1))); 			//
		while((HT_I2C1 -> SR & (1<<19)));			//check if i2c busy	
		HT_I2C1 -> TAR &=~ (1<<10);          	//set mode write
		HT_I2C1 -> TAR |= (0x68);            	//write slave adress in tar
		while(!(HT_I2C1 -> SR & (1<<0)));			//
		while(!(HT_I2C1 -> SR & (1<<2)));	  	// wait until ADRS = 1
		while(!(HT_I2C1 -> SR & (1<<17)));    //wait Data register I2CDR not empty
		while(!(HT_I2C1 -> SR & (1<<19))); 		//
		while(!(HT_I2C1 -> SR & (1<<21))); 		//
		HT_I2C1 ->DR = address;             	//data adress second
		while(!(HT_I2C1 -> SR & (1<<17)));  	//wait Data register I2CDR not empty
		HT_I2C1 -> DR = data;               	// write second
		while(!(HT_I2C1 -> SR & (1<<17)));
		HT_I2C1 -> CR |= (1<<1);              // stop bit
}
int16_t readReg(unsigned char address)
{
		while((HT_I2C1 -> SR & (1<<1))); 			//
		while((HT_I2C1 -> SR & (1<<19)));			// check if i2c busy	
		HT_I2C1 -> TAR &=~ (1<<10);          	// set mode write
		HT_I2C1 -> TAR |= (0x68);            	// write slave adress in tar
		while(!(HT_I2C1 -> SR & (1<<0)));			//
		while(!(HT_I2C1 -> SR & (1<<2)));	  	// wait untill the slave send ACK
		while(!(HT_I2C1 -> SR & (1<<17)));    // wait Data register I2CDR not empty
		while(!(HT_I2C1 -> SR & (1<<19))); 		//
		while(!(HT_I2C1 -> SR & (1<<21))); 		//
		HT_I2C1 -> DR = address;               // data adress second
		HT_I2C1 -> TAR |= (1<<10);           	// write read mode
		HT_I2C1 -> TAR |= (0x68);            	// write slave adress in tar
		while(!(HT_I2C1 -> SR & (1<<2)));     // wait untill the slave send ACK
		while(!(HT_I2C1 -> SR & (1<<19))); 		//
		HT_I2C1 -> CR &=~(1<<0); 							// set bit AA = 0 
		while(!(HT_I2C1 -> SR & (1<<16)));    // check buffer full
		while(!(HT_I2C1 -> SR & (1<<19))); 		// 
		int16_t value = HT_I2C1 -> DR;        // read second
		HT_I2C1 -> CR |= (1<<1);              // stop bit
		
		return value;	
}
void MPC_init(void)
{
		//	writeReg(0x19,0x00);  /* Write to sample rate register */
		//	writeReg(0x1A,0x00);  /* Write to Configuration register */
		//	writeReg(0x1B,0x00);  /* Full scale range +/- 1000 degree/C */
		//	writeReg(0x1C,0x00);
		//	writeReg(0x6B,0x00);
		//sample rate divider(0x19)
		//configuration (0x1a)
		//gyro configuration (0x1b) 0x10
		//acce configuration(0x1c)
		//interrupt(0x38)
		// power managment(0x6b)
		// register 25,26,27,28,56,107
		// register 25(address:0x19) -> 0x07
			writeReg(0x19,0x00);
			writeReg(0x1A,0x00); 
}

int main(void)
{
		///////////////////////////////////////////
		// Enable SPI clock for I2C1
		HT_CKCU -> APBCCR0 |= (1 << 1);
		// Enable clock for AFIO
		HT_CKCU -> APBCCR0 |= (1 << 14);
		//Enable CLK I2C1
		HT_CKCU -> APBCCR0 |= (3 << 14);   
		// Assign AF7(I2C) to PA0, PA1
		HT_AFIO->GPACFGR[0] |= (7<<0);   	// SCL(PA0)
		HT_AFIO->GPACFGR[0] |= (7<<4);   	// SDA(PA1)	  
		//Enable Clock for GPIO - PORT A
		//HT_CKCU->AHBCCR |= (1<<16);
		// Specifies the APB clock prescaler for I2C1
		HT_CKCU -> APBPCSR0 |= (1<<2);//  AHB/2
		//init_Clock();
		I2C_init();
		MPC_init();
		while(1)
		{
				//
				int16_t temp_low = readReg(0x42);
				int16_t temp_high = readReg(0x41);
				int16_t temp = (temp_high << 8) | temp_low;
		}
}
