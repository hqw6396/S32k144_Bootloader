#include "include.h"  

static void WDOG_disable (void){
		WDOG->CNT=0xD928C520; 	  /* Unlock watchdog */
		WDOG->TOVAL=0x0000FFFF;	  /* Maximum timeout value */
		WDOG->CS = 0x00002100;    /* Disable watchdog */
}

static void Hal_System_Led_Confg_Init(void)
{	
		PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK; // enable clock for porte
		PORTD->PCR[1] |= PORT_PCR_MUX(1);	// genral io
		PTD->PDDR |= (1 << 1); // direction : output
		Bsp_Gpio_LowPin(PTD,1);
}

void Hal_Clock_System_Init(void)
{
		WDOG_disable();
		SOSC_init_8MHz();       
		SPLL_init_160MHz();     
		NormalRUNmode_80MHz();  
		SystemCoreClockUpdate();
		Hal_System_Led_Confg_Init();	
}
	
	

	
