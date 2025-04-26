#include "include.h"           

void Test_Delay_Ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < 5000; j++) {
        }
    }
}


static void WDOG_disable (void){
		WDOG->CNT=0xD928C520; 	  /* Unlock watchdog */
		WDOG->TOVAL=0x0000FFFF;	  /* Maximum timeout value */
		WDOG->CS = 0x00002100;    /* Disable watchdog */
}

static void Led_Confg_Init(void)
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

int main(void)
{
		WDOG_disable();
		SOSC_init_8MHz();       /* Initialize system oscillator for 8 MHz xtal */
		SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
		NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
		SystemCoreClockUpdate();
		Led_Confg_Init();	
	
	  Hal_Flash_Init();
//		__asm("cpsid i");
//		__asm("cpsie i");
    Hal_Write_Flash_Data_Function();
		while(1)
		{       
		  Bsp_Gpio_TogglePin(PTD,1);			
       __NOP();
			Test_Delay_Ms(100);
		}
}



