#include "include.h" 


uint8_t FlashCacheData[4096]={0};
static bool Uds_Brushing_Completed_Flag=false;
static uint8_t __attribute__((unused)) UdsUpgradeOtaObjectMode=0;

static uint8_t  __attribute__((unused)) m_udsTxBuffer[512];
static enum_udsSession m_udsCurrentSession = udsSession_Defalut;
//static enum_udsSecurityLevel m_currentSecurityLevel = udsSecurityLevel_None;
//static enum_udsSecurityLevel m_SeedGetLevel = udsSecurityLevel_None;

typedef int16_t ( *pService22_DIDFunction )( uint8_t *pReadData,uint16_t *pDataLength);

static void  __attribute__((unused)) TransmitNegative78ResponseData(uint8_t serviceId)
{
  uint8_t responseData[8];
  responseData[0] = 0x7F;
  responseData[1] = serviceId;
  responseData[2] = 0x78;
  Uds_CanDataTransmit_Function(m_uds_Parameter_Configuration.physicalResponseId,responseData,3,0);
}

static uint8_t Serivice0x10Process(uint8_t *pTpData,uint32_t dataLength,uint8_t functionalAddress)
{
  uint8_t negativeNum;
  uint8_t subFunction;
  uint8_t posRspMsgIndication;
  uint8_t responseData[20];
  
  negativeNum = 0;  
  subFunction = pTpData[1]&0x7F;
  posRspMsgIndication = (pTpData[1]>>7)&0x01;
  
  if(0x01==subFunction)//default
  {    
    if(posRspMsgIndication==0)
    {
      responseData[0] = 0x50;
      responseData[1] = 0x01;
      responseData[2] = 0x00;
      responseData[3] = 0x32;
      responseData[4] = 0x01;
      responseData[5] = 0xf4;
      Uds_CanDataTransmit_Function(m_uds_Parameter_Configuration.physicalResponseId,responseData,6,functionalAddress);
      //delay_ms(50);
      //SystemHardReset();
    }
  }
  else if(0x02==subFunction)//programing
  {
    if((m_udsCurrentSession==udsSession_Extended) || (m_udsCurrentSession==udsSession_Defalut)||(m_udsCurrentSession==udsSession_Programing))
    {
        Uds_Tp_ResetSession_Timer();
        m_udsCurrentSession = udsSession_Programing;
        if(posRspMsgIndication==0)
        {
          responseData[0] = 0x50;
          responseData[1] = 0x02;
          responseData[2] = 0x00;
          responseData[3] = 0x32;
          responseData[4] = 0x01;
          responseData[5] = 0xf4;      
          Uds_CanDataTransmit_Function(m_uds_Parameter_Configuration.physicalResponseId,responseData,6,functionalAddress);
        }         
    }
    else
    {
      negativeNum = 0x22;
    } 
  }
  else if(0x03==subFunction)//extended
  {
    //Uds_Tp_ResetSession_Timer();
		responseData[0] = 0x50;
		responseData[1] = 0x03;
		responseData[2] = 0x00;
		responseData[3] = 0x32;
		responseData[4] = 0x01;
		responseData[5] = 0xf4;      
		Uds_CanDataTransmit_Function(m_uds_Parameter_Configuration.physicalResponseId,responseData,6,functionalAddress);
  }
  else 
  {
    negativeNum = 0x12;//sub function not supported
  }
  Uds_Tp_ResetSession_Timer();
  return negativeNum;
}

static uint8_t Serivice0x11Process(uint8_t *pTpData,uint32_t dataLength,uint8_t functionalAddress)
{
  uint8_t negativeNum;
  uint8_t subFunction;
  uint8_t posRspMsgIndication;
  uint8_t responseData[20];
  
  negativeNum = 0;  
  subFunction = pTpData[1]&0x7F;
  posRspMsgIndication = (pTpData[1]>>7)&0x01;
  
  if(0x01==subFunction)
  {
    if(posRspMsgIndication==0)
    {
      responseData[0] = 0x51;
      responseData[1] = 0x01;
      Uds_CanDataTransmit_Function(m_uds_Parameter_Configuration.physicalResponseId,responseData,2,functionalAddress);
     // delay_ms(50);
     // SystemHardReset();
    }
  }
  else if(0x02==subFunction)
  {
    if(posRspMsgIndication==0)
    {
      responseData[0] = 0x51;
      responseData[1] = 0x02;
      Uds_CanDataTransmit_Function(m_uds_Parameter_Configuration.physicalResponseId,responseData,2,functionalAddress);
    }    
  }
  else if(0x03==subFunction)
  {
    if(posRspMsgIndication==0)
    {
      responseData[0] = 0x51;
      responseData[1] = 0x03;
      Uds_CanDataTransmit_Function(m_uds_Parameter_Configuration.physicalResponseId,responseData,2,functionalAddress);      
     // delay_ms(50);
     // SystemSoftReset();     
    }    
  }
  else 
  {
    negativeNum = 0x12;//sub function not supported
  }
  
  return negativeNum;
}

static uint8_t Serivice0x22Process(uint8_t *pTpData,uint32_t dataLength,uint8_t functionalAddress)
{
    uint8_t negativeNum;
    uint8_t responseData[64];
    uint16_t did;
    uint32_t length = 0;

	  char add[10]="V1.01";
	
    did = (pTpData[1]<<8)+pTpData[2];
    negativeNum = 0;
	
		if(0x6666==did)//Active Diagnostic Session
		{
	    memcpy((uint8_t*)&responseData[3],add,8);
			length = 8;
		}
    else
    {
      negativeNum = 0x31;
    }
    if(0==negativeNum)
    {
        responseData[0] = 0x62;
        responseData[1] = pTpData[1];
        responseData[2] = pTpData[2];
        Uds_CanDataTransmit_Function(m_uds_Parameter_Configuration.physicalResponseId,responseData,length+3,functionalAddress);
    }

    Uds_Tp_ResetSession_Timer();
    return negativeNum;
}

uint16_t Uds_36_Service_SectorNum=0;
uint16_t Uds_36_Service_MemorySizeNum=0;

static uint8_t Serivice0x34Process(uint8_t *pTpData,uint32_t dataLength,uint8_t functionalAddress)
{
  uint8_t negativeNum;
  uint8_t responseData[20];
  int8_t lengthFormat;
  int8_t addressFormat;
  uint32_t address;
  uint32_t size;
  negativeNum = 0;
  
  lengthFormat = (pTpData[2]>>4)&0x0F;
  addressFormat = pTpData[2]&0x0F;
  responseData[0] = 0x74;
  
//  if(m_udsCurrentSession==udsSession_Defalut)
//  {
//    negativeNum = 0x33;
//    return negativeNum;
//  }
 
  if((addressFormat>0)&&(addressFormat<=4))
  {
    if((lengthFormat>0)&&(lengthFormat<=4))
    {
        int8_t i,index;
        uint16_t maxNumberOfBlockLength;
        address = 0;
        size = 0;
        index = 3;
        for(i=addressFormat-1;i>=0;i--)
        {
          address |= (uint32_t)pTpData[index]<<(i*8);
          index++;
        }
        for(i=lengthFormat-1;i>=0;i--)
        {
          size |= (uint32_t)pTpData[index]<<(i*8);
          index++;
        }

        responseData[1] = 0x20;
        maxNumberOfBlockLength = 0x0202;//to be changed
        responseData[2] = (maxNumberOfBlockLength>>8)&0xFF;
        responseData[3] = maxNumberOfBlockLength&0xFF;          
          
	    	Uds_36_Service_MemorySizeNum=0;
	      Uds_36_Service_SectorNum=0;
        Uds_CanDataTransmit_Function(m_uds_Parameter_Configuration.physicalResponseId,responseData,4,functionalAddress); 
    }
    else
    {
      negativeNum = 0x13;//frame length error
    }
  }
  else
  {
    negativeNum = 0x13;//frame length error
  } 
  Uds_Tp_ResetSession_Timer();
  return negativeNum;
}

static uint8_t Serivice0x36Process(uint8_t *pTpData,uint32_t dataLength,uint8_t functionalAddress)
{
  uint8_t negativeNum;
  uint8_t responseData[20];
  uint8_t blockSequenceCounter;
  negativeNum = 0;
  blockSequenceCounter = pTpData[1];
	
	Hal_Uds_36_Service_EraseSector_Write_Flash_Buffer_Function(&pTpData[2],
																														 Uds_36_Service_SectorNum,
																														 256,
																														 Uds_36_Service_MemorySizeNum);
	Uds_36_Service_MemorySizeNum++;
  if(Uds_36_Service_MemorySizeNum>=8)	
	{
	Uds_36_Service_MemorySizeNum=0;
	Uds_36_Service_SectorNum++;
	}
	
	responseData[0] = 0x76;
	responseData[1] = blockSequenceCounter;
	Uds_CanDataTransmit_Function(m_uds_Parameter_Configuration.physicalResponseId,responseData,2,functionalAddress);

  Uds_Tp_ResetSession_Timer();
  return negativeNum;
}

static uint8_t Serivice0x37Process(uint8_t *pTpData,uint32_t dataLength,uint8_t functionalAddress)
{
  uint8_t negativeNum;
  uint8_t responseData[20];
  negativeNum = 0;
 
  responseData[0] = 0x77;
  Uds_CanDataTransmit_Function(m_uds_Parameter_Configuration.physicalResponseId,responseData,1,functionalAddress); 
	Uds_Brushing_Completed_Flag=true;
  Uds_Tp_ResetSession_Timer();

  return negativeNum;
}

static int16_t NegativeResponseProcess(uint8_t negativeNum,uint8_t ServiceId, uint8_t functionalAddress)
{
  uint8_t responseData[8];
  responseData[0] = 0x7F;
  responseData[1] = ServiceId;
  responseData[2] = negativeNum;

  if(functionalAddress && ((negativeNum == 0x11) || (negativeNum == 0x7F) || 
                           (negativeNum == 0x12) ||  (negativeNum == 0x7E)   
                           || (negativeNum == 0x31)))
  {
      return 0;
  }  
  
  Uds_CanDataTransmit_Function(m_uds_Parameter_Configuration.physicalResponseId,responseData,3,0);
  return 0;
}


void UdsServiceProcessFunction(uint8_t *pTpData,uint32_t tpDataLength,uint8_t FunctionalAddress)
{
  uint8_t serviceId;
  uint8_t negativeNum;
  serviceId = pTpData[0];
  negativeNum = 0;
  if(0x10==serviceId)
  {
    negativeNum = Serivice0x10Process(pTpData,tpDataLength,FunctionalAddress);
  }
  else if(0x11==serviceId)
  {
    negativeNum = Serivice0x11Process(pTpData,tpDataLength,FunctionalAddress);
  }
	else if(0x22==serviceId)
  {
    negativeNum = Serivice0x22Process(pTpData,tpDataLength,FunctionalAddress);
  }
	else if(0x34==serviceId)
  {
    negativeNum = Serivice0x34Process(pTpData,tpDataLength,FunctionalAddress);
  }
  else if(0x36==serviceId)
  {
    negativeNum = Serivice0x36Process(pTpData,tpDataLength,FunctionalAddress);
  }
	else if(0x37==serviceId)
  {
    negativeNum = Serivice0x37Process(pTpData,tpDataLength,FunctionalAddress);
  }
	
	
  if(negativeNum!=0)
  {
    NegativeResponseProcess(negativeNum,serviceId,FunctionalAddress);
  }

}

bool Uds_Ret_Brushing_Completed_Flag(void)
{
return Uds_Brushing_Completed_Flag;
}
	
	




