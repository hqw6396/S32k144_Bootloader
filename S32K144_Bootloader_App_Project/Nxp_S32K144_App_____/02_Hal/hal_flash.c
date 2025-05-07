#include "include.h" 

#define HAL_CHECK_APP_START_FLAG_ADDRESS		(0x70000)

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

status_t Hal_App_Start_Flash_Data_Check_Function(void)
{
	status_t ret=STATUS_SUCCESS;
	uint8_t* pDataAddress=NULL;
	uint8_t dCharString[8] = {"APPUPOK "};
	pDataAddress=(uint8_t*)(HAL_CHECK_APP_START_FLAG_ADDRESS);
	memcpy(Hal_Flash_Read_buffer, pDataAddress,8); 
	if (memcmp(Hal_Flash_Read_buffer, dCharString, 8) == 0) {
		return ret ;
	} 
	else {
	  memcpy(Hal_Flash_Read_buffer, dCharString,8);
		INT_SYS_DisableIRQGlobal();
		ret = FLASH_DRV_EraseSector(&pSSDConfig, HAL_CHECK_APP_START_FLAG_ADDRESS, 4096);
		if(ret==STATUS_SUCCESS){
		ret = FLASH_DRV_Program(&pSSDConfig,HAL_CHECK_APP_START_FLAG_ADDRESS,sizeof(Hal_Flash_Read_buffer),Hal_Flash_Read_buffer);
		}
		INT_SYS_EnableIRQGlobal();	
		return 	ret;
	}
}

status_t Hal_GoTo_BootLoader_Flash_Erase_Function(void)
{
		status_t ret=STATUS_SUCCESS;
		uint8_t dCharString[8] = {"GOTOBOOT"};
    memcpy(Hal_Flash_Read_buffer, dCharString,8);
		INT_SYS_DisableIRQGlobal();
		ret = FLASH_DRV_EraseSector(&pSSDConfig, HAL_CHECK_APP_START_FLAG_ADDRESS, 4096);
		if(ret==STATUS_SUCCESS){
		ret = FLASH_DRV_Program(&pSSDConfig,HAL_CHECK_APP_START_FLAG_ADDRESS,sizeof(Hal_Flash_Read_buffer),Hal_Flash_Read_buffer);
		}
		INT_SYS_EnableIRQGlobal();
	  __NOP();
	  return  ret;
}
