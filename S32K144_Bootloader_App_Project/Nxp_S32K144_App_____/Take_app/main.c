#include "include.h"           

#define vTask1_PRIO                      0          
#define vTask1_SIZE                      50             
TaskHandle_t vTask1_Handler;           

#define vTask2_PRIO                      2           
#define vTask2_SIZE                      120             
TaskHandle_t vTask2_Handler;           

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

void vTask1(void *pvParameters)
{
	while(1)
	{
			Bsp_Gpio_TogglePin(PTD,1);
			vTaskDelay(500);
	}
}

void vTask2(void *pvParameters)
{
	while(1)
	{
		  Bsp_Gpio_TogglePin(PTD,0);
			vTaskDelay(100);
	}
}

int main(void)
{
	WDOG_disable();
	SOSC_init_8MHz();       /* Initialize system oscillator for 8 MHz xtal */
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	SystemCoreClockUpdate();
		
	Led_Confg_Init();
	
	xTaskCreate(   vTask1,
	              "LED",
	               vTask1_SIZE,
	               NULL,
	               vTask1_PRIO,
	               &vTask1_Handler);
	xTaskCreate(   vTask2,
	              "CAN",
	               vTask2_SIZE,
	               NULL,
	               vTask2_PRIO,
	               &vTask2_Handler);
	
	vTaskStartScheduler();
	
  while(1)
	{           

  }
}



