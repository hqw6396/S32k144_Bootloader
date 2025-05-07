#include "include.h" 

void Hal_Led_Gpio_Confg_Init(void)
{
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK; // enable clock for porte
    PORTD->PCR[0] |= PORT_PCR_MUX(1);	// genral io
    PTD->PDDR |= (1 << 0); // direction : output
    Bsp_Gpio_LowPin(PTD,0);
	
		PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK; // enable clock for porte
		PORTD->PCR[1] |= PORT_PCR_MUX(1);	// genral io
		PTD->PDDR |= (1 << 1); // direction : output
		Bsp_Gpio_LowPin(PTD,1);
}

