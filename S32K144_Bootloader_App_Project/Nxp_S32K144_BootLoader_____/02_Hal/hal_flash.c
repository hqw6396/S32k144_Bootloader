#include "include.h" 

#define HAL_UDS_FLASH_APP1_START_ADDRESS		0x10000
#define HAL_CHECK_APP_START_FLAG_ADDRESS		0x70000

#define HAL_UDS_MEMORY_BUFFER_SIZE   512
#define HAL_FLASH_SECTOR_LENGHT    4096  

uint8_t Hal_Flash_Read_buffer[HAL_FLASH_SECTOR_LENGHT]={0};
	
const flash_user_config_t Flash1_InitConfig0 = {
    .PFlashBase  = 0x00000000U,                     
    .PFlashSize  = 0x00080000U,                     
    .DFlashBase  = 0x10000000U,                    
    .EERAMBase   = 0x14000000U,                     
    .CallBack    = NULL_CALLBACK
};

flash_ssd_config_t pSSDConfig;

void Hal_Flash_Init(void)
{
	FLASH_DRV_Init(&Flash1_InitConfig0, &pSSDConfig);
}

status_t Hal_Uds_36_Service_EraseSector_Write_Flash_Buffer_Function(uint8_t* hexBuffer,
                                                                    uint8_t  sectorNumber,
                                                                    uint16_t blockSequenceCounter,
																																		uint16_t memorySizeNumber)
{
	status_t ret=STATUS_SUCCESS;
	uint8_t* pDataAddress=NULL;
	pDataAddress=(uint8_t*)(HAL_UDS_FLASH_APP1_START_ADDRESS+sectorNumber*4096);
  memcpy(Hal_Flash_Read_buffer, pDataAddress, sizeof(Hal_Flash_Read_buffer)); 
	memcpy((Hal_Flash_Read_buffer+(HAL_UDS_MEMORY_BUFFER_SIZE*memorySizeNumber)), 
	       hexBuffer, 
	       HAL_UDS_MEMORY_BUFFER_SIZE); 
	INT_SYS_DisableIRQGlobal();
	ret = FLASH_DRV_EraseSector(&pSSDConfig, HAL_UDS_FLASH_APP1_START_ADDRESS+sectorNumber*4096, 4096);
	if(ret==STATUS_SUCCESS){
	ret = FLASH_DRV_Program(&pSSDConfig,HAL_UDS_FLASH_APP1_START_ADDRESS+sectorNumber*4096,sizeof(Hal_Flash_Read_buffer),Hal_Flash_Read_buffer);
	}
	INT_SYS_EnableIRQGlobal();	
  return 	ret;
}

status_t Hal_Goto_App_CharString_Flash_Revert_Function(void)
{
	status_t ret=STATUS_SUCCESS;
	uint8_t dCharString[8] = {"APPUPOK "};	
	memcpy(Hal_Flash_Read_buffer, dCharString,8);
	INT_SYS_DisableIRQGlobal();
	ret = FLASH_DRV_EraseSector(&pSSDConfig, HAL_CHECK_APP_START_FLAG_ADDRESS, 4096);
	if(ret==STATUS_SUCCESS){
	ret = FLASH_DRV_Program(&pSSDConfig,HAL_CHECK_APP_START_FLAG_ADDRESS,sizeof(Hal_Flash_Read_buffer),Hal_Flash_Read_buffer);
	}
	INT_SYS_EnableIRQGlobal();	
	return 	ret;
}



uint8_t Hal_Ret_Check_Start_Mode_Flag_Function(void)
{
	uint8_t* pDataAddress=NULL;
	uint8_t dCharString_EntrtBoot[8] = {"GOTOBOOT"};
	uint8_t dCharString_GotoApp[8] = {"APPUPOK "};
	pDataAddress=(uint8_t*)(HAL_CHECK_APP_START_FLAG_ADDRESS);
  memcpy(Hal_Flash_Read_buffer, pDataAddress, sizeof(Hal_Flash_Read_buffer)); 
	
	if (memcmp(Hal_Flash_Read_buffer, dCharString_EntrtBoot, 8) == 0) {
		return 1 ;
	} 
	else if (memcmp(Hal_Flash_Read_buffer, dCharString_GotoApp, 8) == 0) {
	  return 2 ;
	}
	else {
		return 0;
	}
	return 0;
}

