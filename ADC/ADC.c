//Initial ADC
void ADC_Init()
{
	//Config clock for ADC
	HT_CKCU->AHBCCR |=(1<<16); 		//Enable portA clock
	HT_CKCU->APBCCR1 |= (1<<24);	//Clock ADC Enable
	HT_CKCU->APBCCR0 |=(1<<14);		//Clock AFIO
	
	// Set ADC clock
	HT_CKCU->APBCFGR |= (3<<16); 	//ADC Clock PCLK = CK_AHB / 8
	
	// Assign AF2 (ADC) to pin A6
	HT_AFIO->GPACFGR[0] |=(2<<24); 
	
	//Configure ADC module
	HT_ADC->CR &= ~(3<<0); 		// Continous mode
	HT_ADC->CR &= ~(7<<8); 		// One channel conversion
	HT_ADC->LST[0] |= (6<<0); 
	HT_ADC->STR = 78;			// Sampling time
	HT_ADC->TCR |= (1<<0); 		// ADC conversion software trigger bit enable
	HT_ADC->IER |= (1<<24); 	// Enable overwrite interrupt
	//HT_ADC->IER |= (1<<2); 		// ADC cycle end of coversion interrupt is enable
	HT_ADC->CR |= (1<<7); 		// ADC Enable
}

//Read ADC
int ADC_read()
{
	HT_ADC->TSR |= (1<<0); // Start conversion
	while((HT_ADC->DR[0] & (1<<31)));
	return ((HT_ADC->DR[0]) & ~(1<<31));
}

int main(void)
{
	ADC_Init();
	uint32_t test=0;
	while(1)
	{
		test=ADC_read();
	}
}