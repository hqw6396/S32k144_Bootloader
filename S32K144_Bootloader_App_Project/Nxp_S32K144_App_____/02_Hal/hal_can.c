#include "include.h"

#define MSG_BUF_SIZE  4

HAL_CAN_RX_QUEUE_MESSAGE m_Hal_Can_Rx_Queue_Message={0};
static void Hal_CanSetQueueBufferMessage(HAL_CAN_RX_QUEUE_MESSAGE *pCanDriverBuffe,uint16_t canId,uint8_t *pCanData,uint8_t dlc);

const uint32_t s_u32CTRL1_500k =
// 500k 75%
    (CAN_CTRL1_PRESDIV(0) |
     CAN_CTRL1_RJW(3) |
     CAN_CTRL1_PSEG1(4) |
     CAN_CTRL1_PSEG2(2) |
     CAN_CTRL1_BOFFMSK(1) |
     CAN_CTRL1_ERRMSK(1) |
     CAN_CTRL1_CLKSRC(0) |
     CAN_CTRL1_LPB(0) |
     CAN_CTRL1_TWRNMSK(1) |
     CAN_CTRL1_RWRNMSK(1) |
     CAN_CTRL1_SMP(1) |
     CAN_CTRL1_BOFFREC(1) | // auto recover from busoff
     CAN_CTRL1_TSYN(0) |
     CAN_CTRL1_LBUF(1) | // lowest buffer transmit first
     CAN_CTRL1_LOM(0) | // listen-only mode disable
     CAN_CTRL1_PROPSEG(6)
    );


static void Hal_FlexCAN0_Port_Init(void)
{
    PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK; // enable clock for port
    PORTE->PCR[4] |= PORT_PCR_MUX(5);   // PORTE4 can0 rx
    PORTE->PCR[5] |= PORT_PCR_MUX(5); // portE5 can0 tx
}

static void Hal_FlexCan0_Init(void)
{
    uint8_t   i = 0;
    PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK; /* CGC=1: enable clock to FlexCAN0 */
    CAN0->MCR |= CAN_MCR_MDIS_MASK;         /* MDIS=1: Disable module before selecting clock */
    CAN0->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK;  /* CLKSRC=0: Clock Source = oscillator (8 MHz) */
    CAN0->MCR &= ~CAN_MCR_MDIS_MASK;        /* MDIS=0; Enable module config. (Sets FRZ, HALT)*/
    // enter freeze mode
    CAN0->MCR = (CAN0->MCR & ~CAN_MCR_FRZ_MASK) | CAN_MCR_FRZ(1U);
    CAN0->MCR = (CAN0->MCR & ~CAN_MCR_HALT_MASK) | CAN_MCR_HALT(1U);
    /* Wait for entering the freeze mode */
    while (((CAN0->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT) == 0U)
    {}

    CAN0->MCR |= CAN_MCR_RFEN(1)     | /* RXFIFO enabled */
								 CAN_MCR_SRXDIS_MASK | /* Disable self reception */
								 CAN_MCR_IRMQ_MASK   | /* Individual mask register used */
							// CAN_MCR_DMA_MASK    | /* DMA enabled */
							// CAN_MCR_AEN_MASK    | /* AEN=1: Abort enabled */
                 CAN_MCR_IDAM(0)     | /* ID acceptance mode; 0..Format A */
                 CAN_MCR_MAXMB(16);    /* 32 MB used */

    CAN0->CTRL1 = 0; // clear
    CAN0->CTRL1 = s_u32CTRL1_500k;

//  CAN0->CTRL1 |= 0x01 << 6; //busoff recover disabled
    CAN0->CTRL1 &= (~CAN_CTRL1_BOFFREC_MASK); //busoff recover enabled
    //CAN0->CTRL1 |= 0x01<<14;//bus error interrupt enable

    if ((CAN0->MCR & (1 << 29)) != 0) // RX FIFO enable ?
    {
        CAN0->CTRL2 = (CAN0->CTRL2 & ~CAN_CTRL2_RFFN_MASK) | ((1 << CAN_CTRL2_RFFN_SHIFT) & CAN_CTRL2_RFFN_MASK);
    }
    for (i = 24; i < 128; i++)        /* CAN0: clear 24 msg bufs x 4 words/msg buf = 96 words*/
    {
        CAN0->RAMn[i] = 0x00;         /* Clear msg buf word */
    }
    for (i = 0; i < 16; i++)          /* In FRZ mode, init CAN0 32 msg buf filters */
    {
        CAN0->RXIMR[i] = 0x00000000;  /* Check all ID bits for incoming messages */
    }
    CAN0->RXMGMASK = 0x00000000;      /* Global acceptance mask: check all ID bits */
    CAN0->IMASK1 = 0X00000020;        //enable receive and transmit interrupt   1000 0000 0010 0000  MB31_MB6

    // exit freeze mode
    CAN0->MCR = (CAN0->MCR & ~CAN_MCR_HALT_MASK) | CAN_MCR_HALT(0U);
    CAN0->MCR = (CAN0->MCR & ~CAN_MCR_FRZ_MASK) | CAN_MCR_FRZ(0U);

    /* Wait till exit freeze mode */
    while ((CAN0->MCR & CAN_MCR_FRZACK_MASK) != 0U)
    {}
    while ((CAN0->MCR & CAN_MCR_NOTRDY_MASK) != 0U)
    {}
}

static void Hal_FlexCan0_Init_IRQs(void)
{
	  //Message Buffer 0-15 interrupt
    S32_NVIC->ICPR[(uint32_t)(CAN0_ORed_0_15_MB_IRQn) >> 5U] = (uint32_t)(1UL << ((uint32_t)(CAN0_ORed_0_15_MB_IRQn)&(uint32_t)0x1FU));
    S32_NVIC->ISER[(uint32_t)(CAN0_ORed_0_15_MB_IRQn) >> 5U] = (uint32_t)(1UL << ((uint32_t)(CAN0_ORed_0_15_MB_IRQn)&(uint32_t)0x1FU));
    S32_NVIC->IP[CAN0_ORed_0_15_MB_IRQn] = BSP_SYSYEM_SET_INT_PRIO(BSP_CAN_INT_PRIO);
}

void Hal_FlexCan_Init(void)
{
	  Hal_FlexCAN0_Port_Init();
		Hal_FlexCan0_Init();
		Hal_FlexCan0_Init_IRQs();
}


// can0 use mb31 as general send buffer, mb30 as gateway send buffer.
int16_t Hal_CAN0_Send_Data_Function(uint32_t u32CANID, const uint8_t *pu8Data)
{
    HAL_CAN_DATA_UNION data_union_buffer={0};
    uint8_t txCode;
	
		while (1)
    {
        txCode = ((CAN0->RAMn[15 * MSG_BUF_SIZE + 0] >> 24) & 0x0F);     
        if ((txCode == 0x00) || (txCode == 0x08))
        {
            break;  // 发送缓冲区空闲，可以发送新数据
        }  
    }
		
    CAN0->IFLAG1 = 0x00008000;  //MB31

    data_union_buffer.u8Data[3] = pu8Data[0];
    data_union_buffer.u8Data[2] = pu8Data[1];
    data_union_buffer.u8Data[1] = pu8Data[2];
    data_union_buffer.u8Data[0] = pu8Data[3];

    CAN0->RAMn[15 * MSG_BUF_SIZE + 2] = data_union_buffer.u32Data;

    data_union_buffer.u8Data[3] = pu8Data[4];
    data_union_buffer.u8Data[2] = pu8Data[5];
    data_union_buffer.u8Data[1] = pu8Data[6];
    data_union_buffer.u8Data[0] = pu8Data[7];
    CAN0->RAMn[15 * MSG_BUF_SIZE + 3] = data_union_buffer.u32Data;

		CAN0->RAMn[15 * MSG_BUF_SIZE + 0] = 0X0C480000;
		CAN0->RAMn[15 * MSG_BUF_SIZE + 1] = (uint32_t)(u32CANID << 18);
  
    return 0;
}

void CAN0_ORed_0_15_MB_IRQHandler(void)
{
    uint32_t __attribute__((unused)) dummy = 0;
	  uint8_t __attribute__((unused)) dlc = 0;
    uint32_t rx_code = 0;
    uint32_t rx_id = 0;
    uint8_t is_ext = 0;

    uint8_t canData[8];
    HAL_CAN_DATA_UNION data_1;
    HAL_CAN_DATA_UNION data_2;
    if (CAN0->IFLAG1 & 0x20)
    {
        rx_code = CAN0->RAMn[0 * MSG_BUF_SIZE + 0];

        if ((rx_code & (1 << 20)) != 0) // is remote frame ?
        {
            dummy = CAN0->TIMER;
            CAN0->IFLAG1 = 0X00000040;
            return;
        }

        if ((rx_code & (1 << 21)) != 0) // is extend id ?
        {
            is_ext = 1;
        }

        dlc = (uint8_t)((rx_code >> 16) & 0x0f);

        rx_id = CAN0->RAMn[0 * MSG_BUF_SIZE + 1];
        if (is_ext)
        {
            rx_id &= 0x1FFFFFFF;
            rx_id |= ((uint32_t)0x01 << 31);
        }
        else
        {
            rx_id >>= 18;
            rx_id &= 0x7FF;
        }

        data_1.u32Data = CAN0->RAMn[0 * MSG_BUF_SIZE + 2];
        data_2.u32Data = CAN0->RAMn[0 * MSG_BUF_SIZE + 3];

        dummy = CAN0->TIMER;
        CAN0->IFLAG1 = 0X00000020;

        canData[0] = data_1.u8Data[3];
        canData[1] = data_1.u8Data[2];
        canData[2] = data_1.u8Data[1];
        canData[3] = data_1.u8Data[0];

        canData[4] = data_2.u8Data[3];
        canData[5] = data_2.u8Data[2];
        canData[6] = data_2.u8Data[1];
        canData[7] = data_2.u8Data[0];
				
				Hal_CanSetQueueBufferMessage(&m_Hal_Can_Rx_Queue_Message,rx_id,canData,dlc);
    }   	
}

static void Hal_CanSetQueueBufferMessage(HAL_CAN_RX_QUEUE_MESSAGE *pCanDriverBuffe,uint16_t canId,uint8_t *pCanData,uint8_t dlc)
{
	uint8_t number=0;
	INT_SYS_DisableIRQGlobal();
	number=pCanDriverBuffe->msgRxIndexIn;
	pCanDriverBuffe->pMsgBufferRx[number].canId=canId;
	pCanDriverBuffe->pMsgBufferRx[number].dlc=dlc;
	pCanDriverBuffe->pMsgBufferRx[number].canData[0]=pCanData[0];
  pCanDriverBuffe->pMsgBufferRx[number].canData[1]=pCanData[1];
	pCanDriverBuffe->pMsgBufferRx[number].canData[2]=pCanData[2];
	pCanDriverBuffe->pMsgBufferRx[number].canData[3]=pCanData[3];
	pCanDriverBuffe->pMsgBufferRx[number].canData[4]=pCanData[4];
	pCanDriverBuffe->pMsgBufferRx[number].canData[5]=pCanData[5];
	pCanDriverBuffe->pMsgBufferRx[number].canData[6]=pCanData[6];
	pCanDriverBuffe->pMsgBufferRx[number].canData[7]=pCanData[7];
	pCanDriverBuffe->msgRxIndexIn++;
	if(pCanDriverBuffe->msgRxIndexIn>=HAL_CAN_RX_BUFFER_NUMBER){
	   pCanDriverBuffe->msgRxIndexIn=0;
	}
	pCanDriverBuffe->msgRxCount++;
	INT_SYS_EnableIRQGlobal();
}

int8_t Hal_CanDriverReceiveWaitFunction(HAL_CAN_RX_QUEUE_MESSAGE *pCanDriverBuffer,HAL_CAN_RX_DATA_MESSAGE *pMsg)
{ 
	uint8_t number=0;
  INT_SYS_DisableIRQGlobal();
	if(pCanDriverBuffer->msgRxCount==0){     
			INT_SYS_EnableIRQGlobal();
			return -1;
	}        
  number=pCanDriverBuffer->msgRxIndexOut;
  pMsg->canId=pCanDriverBuffer->pMsgBufferRx[number].canId;
	pMsg->dlc=pCanDriverBuffer->pMsgBufferRx[number].dlc;
	pMsg->canData[0]=pCanDriverBuffer->pMsgBufferRx[number].canData[0];
	pMsg->canData[1]=pCanDriverBuffer->pMsgBufferRx[number].canData[1];
	pMsg->canData[2]=pCanDriverBuffer->pMsgBufferRx[number].canData[2];
	pMsg->canData[3]=pCanDriverBuffer->pMsgBufferRx[number].canData[3];
	pMsg->canData[4]=pCanDriverBuffer->pMsgBufferRx[number].canData[4];
	pMsg->canData[5]=pCanDriverBuffer->pMsgBufferRx[number].canData[5];
	pMsg->canData[6]=pCanDriverBuffer->pMsgBufferRx[number].canData[6];
	pMsg->canData[7]=pCanDriverBuffer->pMsgBufferRx[number].canData[7];
	pCanDriverBuffer->msgRxIndexOut++;
	if(pCanDriverBuffer->msgRxIndexOut>=HAL_CAN_RX_BUFFER_NUMBER){
     pCanDriverBuffer->msgRxIndexOut = 0;
	}
  pCanDriverBuffer->msgRxCount--;
	INT_SYS_EnableIRQGlobal();
	return 0;
}

HAL_CAN_RX_QUEUE_MESSAGE* pt_Hal_Ret_Can_Rx_Queue_Message(void)
{
return &m_Hal_Can_Rx_Queue_Message;
}

void Hal_Task_Can_Tx_Cycle_Function(void){
static uint8_t CanRxBuffer[8]={0,1,2,3,4,5,6,7};	
Hal_CAN0_Send_Data_Function(0x123,CanRxBuffer);
}



