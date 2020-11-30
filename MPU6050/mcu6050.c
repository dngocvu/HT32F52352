#include "ht32f5xxxx_01.h"          // Device header
#include "math.h"
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
float X_thetal,Y_thetal;
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
volatile uint8_t bcd_to_decimal(uint8_t number) 
	{
  return((number >> 4) * 10 + (number & 0x0F));
}
volatile uint8_t decimal_to_bcd(uint8_t number) 
	{
  return (((number / 10) << 4) + (number % 10));
}
void Spi_int(void)
{   
	   // Mode Master         //CS is control software   //CPOL=0,CPHA=0          	// Selporality 
	HT_SPI0->CR1 |=(1<<14);   HT_SPI0->CR1 &=~(1<<13);   HT_SPI0->CR1 |=(1<<8);   HT_SPI0->CR1 &=~(1<<11);
	    //MSB Out Firt        // Enable Select Slave  //SPI clock prescaler   	//Enable Spi 
	HT_SPI0->CR1 &=~(1<<12);  HT_SPI0->CR0 |=(1<<3);  HT_SPI0->CPR = 0;      HT_SPI0->CR0 |=(1<<0);
}
void send_max7219(uint8_t addr, uint8_t data)
{
	HT_SPI0->CR0 |=(1<<4); 
  HT_SPI0->DR=(addr<<8)|data ;
	while (((HT_SPI0->SR)&(1<<1))==0); 
	HT_SPI0->CR0 &=~(1<<4); 
}
void Configure_MAX7219(void)
{ 
	//REG_DECODE_MODE          //REG_INTENSITY 	           //REG_SCAN_LIMIT 
send_max7219(0x09, 0xff);   send_max7219(0x0A, 0x07);   send_max7219(0x0B, 0x07);
  //REG_SHUTDOWN             //REG_DISPLAY_TEST 
send_max7219(0x0C, 0x01);   send_max7219(0x0F, 0x00); 
}

void delecte_max7219(void)
{
		for(k=1;k<=8;k++)
		{ 
			send_max7219(k,0);
		}	
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
	HT_GPIOC->SRR|=(3<<14);
	HT_I2C1->DR=data1;               // write second
	HT_I2C1->CR|=(1<<1);              // stop bit
}
uint16_t Read_Reg(unsigned char  adress)
	{
		uint16_t data;
		while((HT_I2C1->SR&(1<<1))); 
		while((HT_I2C1->SR&(1<<19)));			//check if i2c busy	
		HT_GPIOC->SRR|=(3<<14);
		HT_I2C1->TAR &=~(1<<10);          //set mode write
		HT_I2C1->TAR |=(0x68);            //write slave adress in tar
		while(!(HT_I2C1->SR & (1<<0)));
		while(!(HT_I2C1->SR & (1<<2)));	  // wait untill the slave send ACK
		while(!(HT_I2C1->SR&(1<<17)));    //wait Data register I2CDR not empty
		while(!(HT_I2C1->SR&(1<<19))); 
		while(!(HT_I2C1->SR&(1<<21))); 
		HT_I2C1->DR=adress;                 //data adress second
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
	X_thetal=(float)((atan2((float)Xa1,sqrt(Ya1*Ya1+Za1*Za1))*180)/3.14);
	Y_thetal=(float)((atan2((float)Ya1,sqrt(Xa1*Xa1+Za1*Za1))*180)/3.14);	
}
void Dislay_Read(void)
	{

	}
int main(void)
{
	HT_CKCU->AHBCCR |= (7<<17); //
	HT_CKCU->APBCCR0 |= (3<<14);//
	HT_GPIOC->DIRCR |= (3<<14); //
	HT_GPIOC->DIRCR |= (1<<1);	//
	HT_GPIOC->DOUTR = 0xffff;//
	
	HT_CKCU->APBCCR0 |= (1<<1);
	HT_AFIO->GPACFGR[0] |= (7<<0);   //Touch_SCL(PA0)
	HT_AFIO->GPACFGR[0] |= (7<<4);   //Touch_SDA(PA1)
	HT_CKCU->APBCCR0 |= (3<<14);     //Enable CLK I2C1
	HT_CKCU->APBCCR0 |= (1<<14);     //Enable CLK I2C1 AFIO
	HT_CKCU->APBPCSR0 |= (3<<2);     //I2C1, PCLK = AHB/2 = 24 MHz
	I2C_int();
	
	HT_CKCU->APBCCR0 |= (1<<4) ;     //SPI0
	HT_CKCU->APBPCSR0 |= (1<<4); // SPI0, PCLK = AHB/8 = 6 MHz
	HT_AFIO->GPACFGR[0] |= (5<<16); //SPI0_SCK_PA4
	HT_AFIO->GPACFGR[0] |= (5<<20); //SPI0_MOSI_PA5
	HT_AFIO->GPACFGR[0] |= (5<<28); //SPI0_SEL_PA7
	Spi_int();
	
	Configure_MAX7219();
	delecte_max7219();
	SysTick_Config(48000);
	Mpu_6050_int();
	 
while(1)
	{
	//Dislay_Read();
	//Delay(200); 
	Red_mpu6050();
	//HT_GPIOC->RR|=(3<<14);
	Delay(50);
	//HT_GPIOC->SRR|=(3<<14);
	//Delay(500);	
 }
	
}
