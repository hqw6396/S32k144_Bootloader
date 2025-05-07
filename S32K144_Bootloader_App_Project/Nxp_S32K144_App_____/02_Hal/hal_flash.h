#ifndef HAL_FLASH_H_
#define HAL_FLASH_H_
#include "include.h"

void Hal_Flash_Init(void);
status_t Hal_App_Start_Flash_Data_Check_Function(void);
status_t Hal_GoTo_BootLoader_Flash_Erase_Function(void);
#endif 
