/*************************************************************************************************************
 * @file    GPIO/main.c
 * @version $Rev::              $
 * @date    $Date::            #$
 * @brief   Main program.
 *************************************************************************************************************/
//#include "ht32f520xx_01.h"              // Device header
#include "ht32f5xxxx_01.h"              // Device header

 
 
 
 int main(void)
 {
	 // Enable Clock for PORT C in AHBCCR register
	 HT_CKCU->AHBCCR |= (1<<18);  //enable Port C clock
	
	 // Configure PC14, PC15 as output pins
	 HT_GPIOC->DIRCR |= (1<<14);
	 HT_GPIOC->DIRCR |= (1<<15);
	 HT_GPIOC->DOUTR = 0xFFFF;  // initial value
	 
	 HT_GPIOC->RR |= (1 << 14);   // reset pin PC14
	 HT_GPIOC->RR |= (1 << 15);   // reset pin PC15
 

	 while(1)
	 {
		  
	 }
}
 