#ifndef HAL_CAN_H_
#define HAL_CAN_H_

#include "include.h" 

#define HAL_CAN_RX_BUFFER_NUMBER 30

typedef union
{
	uint32_t	u32Data;
	uint16_t	u16Data[2];
	uint8_t	  u8Data[4];
}HAL_CAN_DATA_UNION;

typedef struct
{
		uint32_t canId;
		uint8_t  canData[8];
		uint8_t  dlc;
}HAL_CAN_RX_DATA_MESSAGE;

typedef struct
{
		uint8_t msgRxCount;
		uint8_t msgRxIndexIn;
		uint8_t msgRxIndexOut;
    HAL_CAN_RX_DATA_MESSAGE pMsgBufferRx[HAL_CAN_RX_BUFFER_NUMBER];	
}HAL_CAN_RX_QUEUE_MESSAGE;

void Hal_FlexCan_Init(void);
int16_t Hal_CAN0_Send_Data_Function(uint32_t u32CANID, const uint8_t *pu8Data);
HAL_CAN_RX_QUEUE_MESSAGE* pt_Hal_Ret_Can_Rx_Queue_Message(void);
int8_t Hal_CanDriverReceiveWaitFunction(HAL_CAN_RX_QUEUE_MESSAGE *pCanDriverBuffer,HAL_CAN_RX_DATA_MESSAGE *pMsg);
#endif 



