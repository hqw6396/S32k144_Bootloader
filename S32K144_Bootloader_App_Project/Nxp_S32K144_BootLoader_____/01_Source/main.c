#include "include.h"           

#define MAIN_APP_START_ADDRESS  (0x10000)

typedef void (*bootloader_fun)(void);
bootloader_fun jump2app;

static uint16_t Main_Led_Timer_Count=0;
static void Main_Led_TogglePin_Function(void)
{
	Main_Led_Timer_Count++;
	if(Main_Led_Timer_Count>=30000){
		Main_Led_Timer_Count=0;
		Bsp_Gpio_TogglePin(PTD,1);
	}		
}

static void Main_Jump_To_App_Function(void)
{
	if(Uds_Ret_Brushing_Completed_Flag()==true)
	{
		__asm("cpsid i")
		jump2app = (bootloader_fun)*(uint32_t *)(MAIN_APP_START_ADDRESS+4);
		jump2app();
	}
}

static void Main_Start_Mode_Function(void)
{
	  if(Hal_Ret_Check_Start_Mode_Flag_Function()==2)
		{
			__asm("cpsid i")
			jump2app = (bootloader_fun)*(uint32_t *)(MAIN_APP_START_ADDRESS+4);
			jump2app();
		}
		else if(Hal_Ret_Check_Start_Mode_Flag_Function()==1)
		{
		  static uint8_t Uds_10_service_responseData[8]={0x06,0x50,0x02,0x00,0x32,0x01,0xf4,0xaa};
			Hal_CAN0_Send_Data_Function(0x797,Uds_10_service_responseData);
			if(Hal_Goto_App_CharString_Flash_Revert_Function()==STATUS_SUCCESS)
			{
				__NOP();
			}
		}
}

int main(void)
{   
	  Hal_Clock_System_Init();
	  Hal_Flash_Init();
	  Hal_FlexCan_Init();
	  Main_Start_Mode_Function();
	  while(1)
		{       		
       __NOP();
			Main_Jump_To_App_Function();
      Main_Led_TogglePin_Function();
			Uds_Diagnostic_Protocol_CycleProcess();
		}
}



