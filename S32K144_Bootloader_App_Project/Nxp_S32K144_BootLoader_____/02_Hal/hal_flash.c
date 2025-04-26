#include "include.h" 

#define FLASH_APP1_SUCCESS_FLAG		0xC000
#define LENGHT    32
uint8_t buffer[LENGHT]={0};
	
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

void Hal_Write_Flash_Data_Function(void)
{
	status_t ret;
	for(int i=0;i<LENGHT;i++)
	{
		buffer[i] = 0x8;
	}
	
	INT_SYS_DisableIRQGlobal();
	ret = FLASH_DRV_EraseSector(&pSSDConfig, FLASH_APP1_SUCCESS_FLAG, 4096);
	ret = FLASH_DRV_Program(&pSSDConfig,FLASH_APP1_SUCCESS_FLAG,sizeof(buffer),buffer);
	INT_SYS_EnableIRQGlobal();		
}


