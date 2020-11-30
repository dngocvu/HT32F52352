// receiver character from pc and send back each second

#include "ht32f5xxxx_01.h"              // Device header
#include "stdio.h"

char a = 'h';

char USART_putc(char ch)
{
	// wait till Transmit Data Register Empty
	while(!(HT_USART1 -> SR & (1<<7)));
	HT_USART1 -> DR = ch;
	return ch;
}

char USART_getc(void)
{
	// wait till Receive Data Register > threshold
	while(!(HT_USART1 -> SR & (1<<5)));
	return HT_USART1 -> DR;
}

void USART_put_dec(unsigned int din)
{
	const unsigned int DecTable[10] = {1000000000,100000000,10000000,1000000,
		100000, 10000, 1000, 100, 10, 1};
	int count = 0; // digital count
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
	unsigned int textlen=0; 			// Current text length
	char ch; 							// current character
	do {
		ch = USART_getc(); 				// Get a character from UART
		switch (ch) {
			case 8: 					// Back space
				if (textlen>0) {
					textlen--;
					USART_putc(ch); 	// Back space
					USART_putc(' '); 	// Replace last character with space on console
					USART_putc(ch); 	// Back space again to adjust cursor position
				}
			break;
		case 13: 						// Enter is pressed
			dest[textlen] = 0; 			// null terminate
			USART_putc(ch); 			// echo typed character
			break;
		case 27: 						// ESC is pressed
			dest[textlen] = 0; 			// null terminate
			USART_putc('\n');
			break;
		default: 						// if input length is within limit and input is valid
			if ((textlen<length) & ((ch >= 0x20) & (ch < 0x7F))) 	// valid characters
			{
				dest[textlen] = ch; 	// append character to buffer
				textlen++;
				USART_putc(ch); 		// echo typed character
			}
			break;
		} // end switch
	} 
	while ((ch!=13) && (ch!=27));
	if (ch==27) {
		return 1; 						// ESC key pressed
	} else {
		return 0;						// Return key pressed
	}
}

void BFTM0_IRQHandler(void)
{
	//USART_puts('Hello');
	//USART_put_dec(12);
	USART_putc(a);
	HT_BFTM0->SR = 0;									// clear the BFTM interrupt flag
}

int main(void)
{
	// Enable clock to BFTM0, AFIO and USART1
	HT_CKCU->APBCCR0 |= (1<<14);     					// AFIO
	HT_CKCU->APBCCR0 |= (1<<9);     					// USART1
	HT_CKCU->APBCCR1 |= (1<<16);						// BFTM0
	// Specify APB clock to BFTM and UART
	HT_CKCU->APBPCSR0 &= ~(3<<26);						// USART1, PCLK = AHB = 48 MHz
	HT_CKCU->APBPCSR0 |= (3<<12);						// BFTM0, PCLK = AHB/8 = 6 MHz
	// Assign AF6(UART) to PC0(RX) and PB15(TX)
	HT_AFIO->GPBCFGR[1] |= (6<<28);   					// AFIO -> PB15
	HT_AFIO->GPCCFGR[0] |= (6<<0);						// AFIO -> PC0
	// Configure BFTM
	HT_BFTM0->CNTR = 0;									// reset counter
	HT_BFTM0->CMP = 6000000;      						// 1s timer
	HT_BFTM0->CR = 5;									// enable timer, interrupt, repetitive mode
	// Configure USART
	HT_USART1->DLR = 5000;								// BRD = 5000, Baudrate = 9600
	
	HT_USART1->CR &= ~(1<<11);							// No parity bit
	HT_USART1->CR &= ~(1<<10);							// 1 STOP bit
	HT_USART1->CR |= (1<<8);							// 8-bit length
	HT_USART1->CR |= (1<<4);							// TX enable
	HT_USART1->CR |= (1<<5);							// RX enable
	HT_USART1->CR &= ~(1<<2);							// LSB first
	HT_USART1->CR &= ~(3<<0);							// Normal mode (UART)
	
	//NVIC enable 
	NVIC_EnableIRQ(BFTM0_IRQn);
	
	
	while(1)
	{
		a = USART_getc();
	}
		
}
