#include <stdio.h>
#include <aduc834.h>

#define INTERRUPT_Timer_2_Overflow	5
#define INTERRUPT_ADC 			    6

int conv_val1;
int conv_val2;
int conv_val3;

void ConFigPin(void);
void ConfigCPU(void);
void WDTConfig(void);
void ConfigSerial(void);
void ConFigADC(void);
void ConFigTimer2(void);
void Transmit(void);
void LMP(void);			          

void main()
{
	int i=0;
	ConFigPin();
	ConfigCPU();
	WDTConfig();
	ConfigSerial();
	ConFigADC();
	ConFigTimer2();

	EA=1;                           // enable global interrupt
    
	while(1)
	{
		LMP();			   			// low-power mode
	}
}

void ConFigPin(void)
{
	T0=0;
}

void ConFigADC(void)
{
    
	ADC0CON = 0xF;	 	        // use of primary ADC, internal reference
	SF = 0xFF;
	EADC=1;                    	// enable ADC interrupt
}

void ConFigCPU(void)
{
  	PLLCON=0x3; 		   		// clock system configure - 1.5MHZ
}

void WDTConfig(void)
{
	WDCON=0x2;		   			// no WDT
}
void ConFigTimer2(void)
{								
	 T2CON=0x00;		   		// T2 configuration
	 TL2=0x00;		   			// reload values
	 RCAP2L=0x00;
	 TH2=0x00;
	 RCAP2H=0x00;
	 TR2=1;                    	// start T2
	 ET2=1;                    	// enable T2 interrupt
}
void LMP(void)
{				   				// low-power mode during cycles
     PCON |=0x01;
}

void ConfigSerial(void)
{
                   
	SCON  = 0x42;			   		// UART configuration
	TMOD |=0x20;                    // set timer 1 in mode 2 8bit auto-reload
	TH1   = 0xF9;                   // reload value for timer 1, 600 baud
	TR1   = 1;			   			// timer 1 GO

}

void Transmit(void)
{
	int i;
	for(i=0;i<1000;i++)		   		// delay loop
		;
	if(RI==1)			   			// check if data is received
	{
		RI=0;			   			//clear receive flag
	}			  					// clear send flag
	else
		 TI=0;
}

/*ISR for timer 2 */
void X(void) interrupt INTERRUPT_Timer_2_Overflow
{
	TF2=0;							// Clear T2 IR flag
	T0=1;							// LED off
	ADCMODE=0x22;                   // enable ADC in single conversion mode
}

/*ADC ISR */
void adc_int(void) interrupt INTERRUPT_ADC
{
	int i;
	ADCMODE |=0x2;            	     // Stop ADC
	RDY0=0;				             // clear interrupt flag of ADC
	conv_val1=ADC0H;		  	     // saves value of conversion
	conv_val2=ADC0M;
	conv_val3=ADC0L;
	
	T0=0;			                 // LED on
	for(i=0;i<3;i++)
	{
		if(i==0)					 // transmit data
		{
			SBUF=conv_val1;
			Transmit();
		}
		else if(i==1){
			SBUF=conv_val2;
			Transmit();
		}
		else if(i==2){
			SBUF=conv_val3;
			Transmit();
		}
	}
}

