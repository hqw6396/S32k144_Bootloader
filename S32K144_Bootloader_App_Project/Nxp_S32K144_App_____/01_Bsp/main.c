#include "include.h"           

#define vTask1_PRIO                      0          
#define vTask1_SIZE                      256            
TaskHandle_t vTask1_Handler;           

#define vTask2_PRIO                      1           
#define vTask2_SIZE                      256             
TaskHandle_t vTask2_Handler;           


void vTask1(void *pvParameters)
{
	while(1)
	{
		  Hal_Task_Can_Tx_Cycle_Function();
			Bsp_Gpio_TogglePin(PTD,0);
			vTaskDelay(500);
	}
}

void vTask2(void *pvParameters)
{
	while(1)
	{
			Uds_Diagnostic_Protocol_CycleProcess();
			vTaskDelay(5);
	}
}

int main(void)
{
	Bsp_Wdog_Disable_Init();
  Bsp_System_Clock_Init();		
  Hal_Led_Gpio_Confg_Init();
	Hal_FlexCan_Init();
	Hal_Flash_Init();
	Hal_App_Start_Flash_Data_Check_Function();
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



