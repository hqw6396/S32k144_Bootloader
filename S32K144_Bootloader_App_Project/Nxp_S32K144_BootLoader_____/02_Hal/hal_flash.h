#ifndef HAL_FLASH_H_
#define HAL_FLASH_H_
#include "include.h"

void Hal_Flash_Init(void);
status_t Hal_Uds_36_Service_EraseSector_Write_Flash_Buffer_Function(uint8_t* hexBuffer,
                                                                    uint8_t  sectorNumber,
                                                                    uint16_t blockSequenceCounter,
																																		uint16_t memorySizeNumber);
uint8_t Hal_Ret_Check_Start_Mode_Flag_Function(void);
status_t Hal_Goto_App_CharString_Flash_Revert_Function(void);
#endif 
