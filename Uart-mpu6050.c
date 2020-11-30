#include "ht32f5xxxx_01.h"          // Device header
#include "math.h"
#include "stdio.h"
int k;
int dem;
volatile uint32_t msTicks = 0;
int16_t Xa=0,Ya=0,Za=0;
float Xa1,Ya1,Za1;
int16_t temp=0;
float temp1;
int16_t adr;
int16_t Xg=0,Yg=0,Zg=0;
float Xg1,Yg1,Zg1;
int16_t X_thetal,Y_thetal;
void Delay(uint32_t dlyTicks)
{
	uint32_t curTicks;
	curTicks = msTicks;
	while((msTicks-curTicks)<dlyTicks);
}
void SysTick_Handler(void)
{
	msTicks++;
}

void I2C_int(void)
{                                                // Enable I2C1	
  HT_I2C1->SHPGR =24;   HT_I2C1->SLPGR =24;   HT_I2C1->CR |=(1<<3);          
}
void Write_Reg(unsigned char adress1,int data1)
{
	//data1=decimal_to_bcd(data1);
	while((HT_I2C1->SR&(1<<1))); 
	while((HT_I2C1->SR&(1<<19)));			//check if i2c busy	
	HT_I2C1->TAR &=~(1<<10);          //set mode write
	HT_I2C1->TAR |=(0x68);            //write slave adress in tar
	while(!(HT_I2C1->SR & (1<<0)));
	while(!(HT_I2C1->SR & (1<<2)));	  // wait untill the slave send ACK
	while(!(HT_I2C1->SR&(1<<17)));    //wait Data register I2CDR not empty
	while(!(HT_I2C1->SR&(1<<19))); 
	while(!(HT_I2C1->SR&(1<<21))); 
	HT_I2C1->DR=adress1;                 //data adress second
	while(!(HT_I2C1->SR&(1<<17)));    //wait Data register I2CDR not empty
	//HT_GPIOC->SRR|=(3<<14);
	HT_I2C1->DR=data1;               // write second
	while(!(HT_I2C1->SR&(1<<17)));
	HT_I2C1->CR|=(1<<1);              // stop bit
}
uint16_t Read_Reg(unsigned char  address1)
	{
		uint16_t data;
		while((HT_I2C1->SR&(1<<1))); 
		while((HT_I2C1->SR&(1<<19)));			//check if i2c busy	
		//HT_GPIOC->SRR|=(3<<14);
		HT_I2C1->TAR &=~(1<<10);          //set mode write
		HT_I2C1->TAR |=(0x68);            //write slave adress in tar
		while(!(HT_I2C1->SR & (1<<0)));
		while(!(HT_I2C1->SR & (1<<2)));	  // wait untill the slave send ACK
		while(!(HT_I2C1->SR&(1<<17)));    //wait Data register I2CDR not empty
		while(!(HT_I2C1->SR&(1<<19))); 
		while(!(HT_I2C1->SR&(1<<21))); 
		HT_I2C1->DR=address1;                 //data adress second
		HT_I2C1->TAR |=(1<<10);           // write read mode
		HT_I2C1->TAR |=(0x68);            //write slave adress in tar
		while(!(HT_I2C1->SR &(1<<2)));     // wait untill the slave send ACK
		while(!(HT_I2C1->SR&(1<<19))); 
		HT_I2C1->CR &=~(1<<0); 
		while(!(HT_I2C1->SR&(1<<16)));    // check buffer full
		while(!(HT_I2C1->SR&(1<<19))); 
		data=HT_I2C1->DR;               // read second
		HT_I2C1->CR|=(1<<1);              // stop bit
		return data;	

	 }
void Mpu_6050_int(void)
{
	Write_Reg(0x19,0x00);  /* Write to sample rate register */
	Write_Reg(0x1A,0x00);  /* Write to Configuration register */
	Write_Reg(0x1B,0x00);  /* Full scale range +/- 1000 degree/C */
	Write_Reg(0x1C,0x00);
	Write_Reg(0x6B,0x00);
//sample rate divider(0x19)
//configuration (0x1a)
//gyro configuration (0x1b) 0x10
//acce configuration(0x1c)
//interrupt(0x38)
// power managment(0x6b)
}
void Red_mpu6050(void)
{
	//ACCELERATION
	Xa=(int16_t)((Read_Reg(0x3B))<<8|Read_Reg(0x3C));
	Ya=(int16_t)((Read_Reg(0x3E))<<8|Read_Reg(0x3F));
	Za=(int16_t)((Read_Reg(0x3D))<<8|Read_Reg(0x40));
	//TEMPERATURE
	temp=(int16_t)((Read_Reg(0x41))<<8|Read_Reg(0x42));
	//GYROSCOPE
	Xg=(int16_t)((Read_Reg(0x43))<<8|Read_Reg(0x44));
	Yg=(int16_t)((Read_Reg(0x45))<<8|Read_Reg(0x46));
	Zg=(int16_t)((Read_Reg(0x47))<<8|Read_Reg(0x48));
	//CONVERT
	Xa1=Xa/16384.0;
	Ya1=Ya/16384.0;
	Za1=Za/16384.0;
	temp1=(float)temp/340 + 36.53;
	Xg1=(float)Xg/131.0;
	Yg1=(float)Yg/131.0;
	Zg1=(float)Zg/131.0;
	adr=(Read_Reg(0x75));
	X_thetal=(int16_t)((atan2((float)Xa1,sqrt(Ya1*Ya1+Za1*Za1))*180)/3.14);
	Y_thetal=(int16_t)((atan2((float)Ya1,sqrt(Xa1*Xa1+Za1*Za1))*180)/3.14);	
}
int dem=500;
char a='c';

char USART_putc(char ch)
{
	// wait till Transmit Data Register Empty
	while(!(HT_USART1->SR & (1<<7)));
	HT_USART1->DR = ch;
	return ch;
}

char USART_getc(void)
{
	// wait till Receive Data Register > threshold
	while(!(HT_USART1->SR & (1<<5)));
	return HT_USART1->DR;
}

void USART_put_dec(unsigned int din)
{
	const unsigned int DecTable[10] = {1000000000,100000000,10000000,1000000,
		100000, 10000, 1000, 100, 10, 1};
	int count=0; // digital count
	int n; // calculation for each digital
	// Remove preceding zeros
	while ((din < DecTable[count]) && (din>10)) {count++;}
	while (count<10) {
		n=0;
		while (din >= DecTable[count]) {
			din = din - DecTable[count];
			n++;
		}
	n = n + 48; // convert to ascii 0 to 9
	USART_putc((char) n);
	count++;
	};
	return;
}

void USART_puts(char *mytext)
{
	char CurrChar;
	CurrChar = *mytext;
	while (CurrChar != (char) 0x0){
		USART_putc(CurrChar); // Normal data
		mytext++;
		CurrChar = *mytext;
	}
	return;
}

int USART_gets(char dest[], int length)
{
	unsigned int textlen=0; // Current text length
	char ch; // current character
	do {
		ch = USART_getc(); // Get a character from UART
		switch (ch) {
			case 8: // Back space
				if (textlen>0) {
					textlen--;
					USART_putc(ch); // Back space
					USART_putc(' '); // Replace last character with space on console
					USART_putc(ch); // Back space again to adjust cursor position
				}
			break;
		case 13: // Enter is pressed
			dest[textlen] = 0; // null terminate
			USART_putc(ch); // echo typed character
			break;
		case 27: // ESC is pressed
			dest[textlen] = 0; // null terminate
			USART_putc('\n');
			break;
		default: // if input length is within limit and input is valid
			if ((textlen<length) & ((ch >= 0x20) & (ch < 0x7F))) // valid characters
			{
				dest[textlen] = ch; // append character to buffer
				textlen++;
				USART_putc(ch); // echo typed character
			}
			break;
		} // end switch
	} 
	while ((ch!=13) && (ch!=27));
	if (ch==27) {
		return 1; // ESC key pressed
	} else {
		return 0; // Return key pressed
	}
}

void BFTM0_IRQHandler(void)
{
	char chuoi[100];
  	Red_mpu6050();
	sprintf(chuoi,"%d %d \n\r",X_thetal,Y_thetal);
	USART_puts(chuoi);
	HT_BFTM0->SR = 0;							// clear the BFTM interrupt flag
}
int main(void)
{
  	HT_CKCU->AHBCCR |= (7<<17); 	//
	HT_CKCU->APBCCR0 |= (3<<14);	//
	HT_GPIOC->DIRCR |= (3<<14); 	//
	HT_GPIOC->DIRCR |= (1<<1);	//
	HT_GPIOC->DOUTR = 0xffff;//
	
	HT_CKCU->APBCCR0 |= (1<<1);
	HT_AFIO->GPACFGR[0] |= (7<<0);   //Touch_SCL(PA0)
	HT_AFIO->GPACFGR[0] |= (7<<4);   //Touch_SDA(PA1)
	HT_CKCU->APBCCR0 |= (3<<14);     //Enable CLK I2C1
	HT_CKCU->APBCCR0 |= (1<<14);     //Enable CLK I2C1 AFIO
  	HT_CKCU->APBPCSR0 |= (3<<2);     //I2C1, PCLK = AHB/2 = 24 MHz
  	I2C_int();

	SysTick_Config(48000);
	Mpu_6050_int();

	HT_CKCU->AHBCCR |= (7<<17); 	//
	HT_CKCU->APBCCR0 |= (3<<14);	//
	HT_GPIOC->DIRCR |= (3<<14); 	//
	HT_GPIOC->DIRCR |= (1<<1);	//
	HT_GPIOC->DOUTR = 0xffff;	//
	 
	// Enable clock to BFTM0, AFIO and USART1
	HT_CKCU->APBCCR0 |= (1<<14);     			// AFIO
	HT_CKCU->APBCCR0 |= (1<<9);     			// USART1
	HT_CKCU->APBCCR1 |= (1<<16);				// BFTM0
	// Specify APB clock to BFTM and UART
	HT_CKCU->APBPCSR0 &= ~(3<<26);				// USART1, 	PCLK = AHB = 48 MHz
	HT_CKCU->APBPCSR0 |= (3<<12);				// BFTM0, 	PCLK = AHB/8 = 6 MHz
	// Assign AF6(UART) to PC0(RX) and PB15(TX)
	HT_AFIO->GPBCFGR[1] |= (6<<28);   			// AFIO -> PB15
	HT_AFIO->GPCCFGR[0] |= (6<<0);				// AFIO -> PC0
	// Configure BFTM
	HT_BFTM0->CNTR = 0;					// reset counter
	HT_BFTM0->CMP = 6000000;      				// 1s timer
	HT_BFTM0->CR = 5;					// enable timer, interrupt, repetitive mode
	// Configure USART
	HT_USART1->DLR = 5000;					// BRD = 5000, Baudrate = 9600
	HT_USART1->CR &= ~(1<<11);				// No parity bit
	HT_USART1->CR &= ~(1<<10);				// 1 STOP bit
	HT_USART1->CR |= (1<<8);				// 8-bit length
	HT_USART1->CR |= (1<<4);				// TX enable
	HT_USART1->CR |= (1<<5);				// RX enable
	HT_USART1->CR &= ~(1<<2);				// LSB first
	HT_USART1->CR &= ~(3<<0);				// Normal mode (UART)
	NVIC_EnableIRQ(BFTM0_IRQn);
	HT_GPIOC->RR |= (3<<14);
	HT_GPIOC->RR |= (1<<1);	

	while(1)
	{

 	}
	
}
