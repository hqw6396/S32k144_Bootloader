#include "include.h" 

static volatile UDS_TP_RECEIVE_DATA_MESSAGE m_Uds_PhysicalRxData={0};
static volatile UDS_TP_RECEIVE_DATA_MESSAGE m_Uds_FunctionalRxData={0};

UDS_TP_VARIABLES_MESSAGE m_Uds_Tp_Variables_Message={0};

const UDS_PARAMETER_MESSAGE m_uds_Parameter_Configuration =
{
  .physicalResponseId  =  0x797,
  .physicalRequestId   =  0x787,
  .functionalRequestId =  0x7DF,
  .fillByte  = 0xAA,
  .blockSize = 0,
  .STmin = 1,
  .flowControlTime = 1000,
};


void Uds_Tp_ResetSession_Timer(void)
{
  m_Uds_Tp_Variables_Message.m_sessionTimerCount = 5000;
}


void Uds_CanTx_FlowControlFrame_Management(uint8_t *canData,uint8_t dataLength,uint8_t functionalAddress)
{
  if(functionalAddress)//not support
  {
    return ;
  }
  if(dataLength<3)
  {
    return ;
  }
  if((canData[0])!=0x30)
  {
    return ;
  }
  m_Uds_Tp_Variables_Message.m_txBlockSize = canData[1];
  m_Uds_Tp_Variables_Message.m_txSTmin = canData[2];
  m_Uds_Tp_Variables_Message.m_txReceiveFCFlag = 1;
}

static void Uds_CanRx_Management_Process(uint8_t *canData,uint8_t dataLength,uint8_t functionalAddress)
{	
	uint8_t u8FcCmd[8] = {0x30,0x00,0x02,0x00,0x00,0x00,0x00,0x00};
	uint8_t lastFrameByte=0;
	volatile UDS_TP_RECEIVE_DATA_MESSAGE *pTpReceive;
	
	if(functionalAddress)
	{
		pTpReceive = &m_Uds_FunctionalRxData;
	}
	else
	{
		pTpReceive = &m_Uds_PhysicalRxData;
	}
		
	if(((canData[0] & 0xF0) == 0x00))//single frame
	{
			pTpReceive->receiveData[0]= canData[1];
			pTpReceive->receiveData[1]= canData[2];
			pTpReceive->receiveData[2]= canData[3];
			pTpReceive->receiveData[3]= canData[4];
			pTpReceive->receiveData[4]= canData[5];
			pTpReceive->receiveData[5]= canData[6];
			pTpReceive->receiveData[6]= canData[7];
			
			pTpReceive->rxMulitFrameNumber = 0;
			pTpReceive->rxMulitFrameCount = 0; 
			
			pTpReceive->receiveLength = canData[0]&0x0F;
			pTpReceive->receiveFlag = 1;		
	}
	else
	{
		if((0x10 == (canData[0] & 0xF0)))//fist frame of the multi frame 
		{
				pTpReceive->rxWaitTimeCounter = 0;
				pTpReceive->rxMulitFrameCount = 0;
				
				pTpReceive->receiveLength = (((uint32_t)canData[0] & 0x0F)<<8)+((uint32_t)canData[1]);
				pTpReceive->rxMulitFrameNumber = (pTpReceive->receiveLength - 6 ) / 7;
				pTpReceive->rxMulitFrameNumber += ((pTpReceive->receiveLength - 6 ) % 7) ? 1 : 0;
				
				memcpy((void*)pTpReceive->receiveData,&canData[2],6);
				
				u8FcCmd[1] = m_uds_Parameter_Configuration.blockSize;
				u8FcCmd[2] = m_uds_Parameter_Configuration.STmin;
				u8FcCmd[3] = m_uds_Parameter_Configuration.fillByte;
				u8FcCmd[4] = m_uds_Parameter_Configuration.fillByte;
				u8FcCmd[5] = m_uds_Parameter_Configuration.fillByte;
				u8FcCmd[6] = m_uds_Parameter_Configuration.fillByte;
				u8FcCmd[7] = m_uds_Parameter_Configuration.fillByte;	
			  Hal_CAN0_Send_Data_Function(m_uds_Parameter_Configuration.physicalResponseId,u8FcCmd);
				pTpReceive->rxMulitFrameFlag = true;
		
		}
		else
		{
			if( pTpReceive->rxMulitFrameNumber )
			{
				if((canData[0]&0xF0)==0x20)//consecutive frame
				{
					pTpReceive->rxWaitTimeCounter = 0;
					pTpReceive->rxMulitFrameCount++;
					if((pTpReceive->rxMulitFrameCount&0x0F)!=(canData[0]&0x0F))
					{
						pTpReceive->rxMulitFrameFlag = false;
						return ;
					}
					if(pTpReceive->rxMulitFrameNumber >= 2)
					{
						memcpy( (void*)&pTpReceive->receiveData[(pTpReceive->rxMulitFrameCount - 1) * 7 + 6], &(canData[1]), 7 ); //??????
						pTpReceive->rxMulitFrameNumber--;
					}
					else
					{
						lastFrameByte = (pTpReceive->receiveLength - 6 ) % 7;
						if(lastFrameByte == 0)
						{
								lastFrameByte = 7;
						}
						memcpy((void*)&pTpReceive->receiveData[(pTpReceive->rxMulitFrameCount - 1) * 7 + 6], &(canData[1]), lastFrameByte ); //??????
						pTpReceive->rxMulitFrameNumber--;
						pTpReceive->rxMulitFrameCount = 0;
						pTpReceive->rxMulitFrameFlag = false;
						pTpReceive->receiveFlag = 1;
					}
				}
			}
			else
			{
						pTpReceive->rxMulitFrameCount = 0; 
						pTpReceive->rxMulitFrameFlag = false;	
			}
		}
			
	}	
}


void Uds_CanDataTransmit_Function(uint32_t canId,uint8_t *pTpData,uint32_t tpDataLength,uint8_t functionAddress)
{
  if(tpDataLength<8)//single frame
  {
    uint8_t data[8];
    uint32_t i;
    data[0] = tpDataLength;
    for(i=0;i<tpDataLength;i++)
    {
      data[i+1] = pTpData[i];
    }
    for(;i<7;i++)
    {
      data[i+1] = m_uds_Parameter_Configuration.fillByte;
    }
    Hal_CAN0_Send_Data_Function(canId,data);
    return;
  }
  else//multi frame
  {
    m_Uds_Tp_Variables_Message.m_TxMulitFrameFlag = 1;
    uint8_t txCanData[8];
    uint8_t txCanDlc;
    
    txCanDlc = 0;
    m_Uds_Tp_Variables_Message.m_bTxMulitFrameNumber = (tpDataLength+1)/(8-1);
    if((tpDataLength + 1) % (8 - 1) != 0)
    {
        m_Uds_Tp_Variables_Message.m_bTxMulitFrameNumber += 1;
    }
    m_Uds_Tp_Variables_Message.m_bTxMulitFrameCount = 0;
    txCanData[0] = (0x10 | (tpDataLength / 256));
    txCanData[1] = (tpDataLength % 256);
    for (int i = 0; i < 6;i++ )
    {
        txCanData[i+2] = pTpData[i];
    }
    m_Uds_Tp_Variables_Message.m_udsTpTransmitStatus = UDS_TP_TRANSMIT_STATUS_FC_TIMEOUT;
    txCanDlc = 8;
    m_Uds_Tp_Variables_Message.m_txReceiveFCFlag = 0;
		Hal_CAN0_Send_Data_Function(canId,txCanData);   
    while(1)
    {
        m_Uds_Tp_Variables_Message.m_txWaitTimeoutCounter = m_uds_Parameter_Configuration.flowControlTime;
        
        while(m_Uds_Tp_Variables_Message.m_txWaitTimeoutCounter!=0)
        {
          if(m_Uds_Tp_Variables_Message.m_txReceiveFCFlag)
          {
            m_Uds_Tp_Variables_Message.m_udsTpTransmitStatus = UDS_TP_TRANSMIT_STATUS_OK;
            break;
          }
        }
        if(UDS_TP_TRANSMIT_STATUS_OK!=m_Uds_Tp_Variables_Message.m_udsTpTransmitStatus)
        {
          m_Uds_Tp_Variables_Message.m_TxMulitFrameFlag = 0;      
          return;
        }
        uint32_t frameInterval;
        if(m_Uds_Tp_Variables_Message.m_txSTmin==0)
        {
          frameInterval = 0;
        }
        else if(m_Uds_Tp_Variables_Message.m_txSTmin<128)
        {
          frameInterval = m_Uds_Tp_Variables_Message.m_txSTmin;
        }
        else
        {
          frameInterval = 1;
        }
        m_Uds_Tp_Variables_Message.m_bTxMulitFrameNumber -= 1;
        for(m_Uds_Tp_Variables_Message.m_bTxMulitFrameCount=0;
				    m_Uds_Tp_Variables_Message.m_bTxMulitFrameCount<m_Uds_Tp_Variables_Message.m_bTxMulitFrameNumber;
				    m_Uds_Tp_Variables_Message.m_bTxMulitFrameCount++)
        {
          txCanData[0] = (0x20 | ((m_Uds_Tp_Variables_Message.m_bTxMulitFrameCount + 1) & 0x0f));
          if (m_Uds_Tp_Variables_Message.m_bTxMulitFrameCount < (m_Uds_Tp_Variables_Message.m_bTxMulitFrameNumber - 1))
          {
              txCanDlc = 8;
          }
          else
          {
              txCanDlc = ((tpDataLength - 6 - m_Uds_Tp_Variables_Message.m_bTxMulitFrameCount * 7) + 1);
          }
          int j;
          for ( j = 0; j < txCanDlc-1; j++)
          {
              txCanData[j + 1] = pTpData[6 + m_Uds_Tp_Variables_Message.m_bTxMulitFrameCount * 7 + j];

          }
          for ( ; j < 7; j++)
          {
              txCanData[j + 1] = m_uds_Parameter_Configuration.fillByte;

          }
					Hal_CAN0_Send_Data_Function(canId,txCanData);
          if(frameInterval!=0)
          {
            //delay_ms(frameInterval);
          }
          if ((m_Uds_Tp_Variables_Message.m_txBlockSize != 0) && ((m_Uds_Tp_Variables_Message.m_bTxMulitFrameCount + 1) >= m_Uds_Tp_Variables_Message.m_txBlockSize))
          {
              break;//block transmit finished
          }
        }
        if(m_Uds_Tp_Variables_Message.m_bTxMulitFrameCount>=m_Uds_Tp_Variables_Message.m_bTxMulitFrameNumber)//transmit successfull
        {
          break;
        }
        m_Uds_Tp_Variables_Message.m_txReceiveFCFlag = 0;
    }
    m_Uds_Tp_Variables_Message.m_TxMulitFrameFlag = 0; 

  }
}

static void Uds_CanRxTp_Callback(uint32_t rxId,uint8_t *canData,uint8_t dlc)
{
    uint32_t canId;
		
		canId = rxId&0x7FFFFFFF;
	
		if((canId==m_uds_Parameter_Configuration.physicalRequestId)||
			 (canId==m_uds_Parameter_Configuration.functionalRequestId))
		{
				if(m_Uds_Tp_Variables_Message.m_TxMulitFrameFlag)
				{
					Uds_CanTx_FlowControlFrame_Management(canData,dlc,0);
				}
			
				Uds_CanRx_Management_Process(canData,dlc,0);           
		}
}

static void Uds_Service_CycleProcess(void)
{
  if(m_Uds_PhysicalRxData.receiveFlag)	//uds tp data valid
  {
    m_Uds_PhysicalRxData.receiveFlag = 0;
    UdsServiceProcessFunction((uint8_t*)m_Uds_PhysicalRxData.receiveData,m_Uds_PhysicalRxData.receiveLength,0);
  }
  if(m_Uds_FunctionalRxData.receiveFlag)	//uds tp data valid
  {
    m_Uds_FunctionalRxData.receiveFlag = 0;
    UdsServiceProcessFunction((uint8_t *)m_Uds_FunctionalRxData.receiveData,m_Uds_FunctionalRxData.receiveLength,1);
  }
}
	

void Uds_Diagnostic_Protocol_CycleProcess(void)
{
	static int ret;
  static HAL_CAN_RX_DATA_MESSAGE uds_can_messsage={0};
  HAL_CAN_RX_QUEUE_MESSAGE *ptCanmessage=pt_Hal_Ret_Can_Rx_Queue_Message();
	while(1)
	{
		ret=Hal_CanDriverReceiveWaitFunction(ptCanmessage,&uds_can_messsage);
		if(ret == 0)   //can receive successful
		{		
        Uds_CanRxTp_Callback(uds_can_messsage.canId,uds_can_messsage.canData,uds_can_messsage.dlc);
			  Uds_Service_CycleProcess();
		}
		else           
		{
				break;
		}
	}
}


