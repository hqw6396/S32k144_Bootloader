#ifndef UDS_TP_H_
#define UDS_TP_H_

#include "include.h"   

#define UDS_TP_TRANSMIT_STATUS_OK	              0
#define UDS_TP_TRANSMIT_STATUS_FC_TIMEOUT       1

typedef struct
{
			volatile  uint8_t  m_TxMulitFrameFlag;
			volatile  uint32_t m_bTxMulitFrameNumber;
			volatile	uint32_t m_bTxMulitFrameCount ;
			volatile	uint32_t m_txWaitTimeoutCounter;
			volatile	uint8_t  m_txReceiveFCFlag;
			volatile	uint8_t  m_txBlockSize;
			volatile	uint8_t  m_txSTmin;
			volatile	int32_t  m_udsTpTransmitStatus;
			uint8_t  m_SecurityAccessTimerFlag;
			uint32_t m_SecurityAccessTimerCount;
			uint8_t  m_securityTimerDelayType;
	    uint32_t m_sessionTimerCount;
} UDS_TP_VARIABLES_MESSAGE;

typedef struct
{
    uint8_t  receiveData[1024];
    uint32_t receiveLength;
    uint8_t  receiveFlag;
    uint32_t rxMulitFrameCount;          
    uint32_t rxMulitFrameNumber;         
    uint8_t  rxMulitFrameFlag;
    uint32_t rxWaitTimeCounter;
} UDS_TP_RECEIVE_DATA_MESSAGE;

typedef struct
{
  uint32_t physicalResponseId;
  uint32_t physicalRequestId;
  uint32_t functionalRequestId;  
  uint8_t  fillByte;
  uint8_t  blockSize;
  uint8_t  STmin;
  uint32_t flowControlTime;
}UDS_PARAMETER_MESSAGE;


//static const UDS_PARAMETER_MESSAGE m_uds_Parameter_Configuration =
//{
//  .physicalResponseId  =  0x797,
//  .physicalRequestId   =  0x787,
//  .functionalRequestId =  0x7DF,
//  .fillByte  = 0xAA,
//  .blockSize = 0,
//  .STmin = 1,
//  .flowControlTime = 1000,
//};
extern const UDS_PARAMETER_MESSAGE m_uds_Parameter_Configuration; 
void Uds_CanDataTransmit_Function(uint32_t canId,uint8_t *pTpData,uint32_t tpDataLength,uint8_t functionAddress);
void Uds_Tp_ResetSession_Timer(void);
void Uds_Diagnostic_Protocol_CycleProcess(void);

#endif 




