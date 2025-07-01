#include "include.h" 

uint8_t TCD0_Source[] = {"Hello World"};  /* TCD0源数据：11字节字符串 */
uint8_t volatile TCD0_Dest[11] = {0};     /* TCD0目标地址：单字节变量 */

/* TCD（传输控制描述符）初始化函数 */
static void Hal_Dma_Tcd_init(void) {
    /*---------------- 源配置 ----------------*/
    DMA->TCD[0].SADDR = (uint32_t)&TCD0_Source;  // 源地址指向字符串首地址
    DMA->TCD[0].SOFF = 1;                        // 每次传输后源地址+1字节
    
    /* 属性配置：SMOD=禁用, SSIZE=1字节, DMOD=禁用, DSIZE=1字节 */
    DMA->TCD[0].ATTR = (0 << DMA_TCD_ATTR_SMOD_SHIFT) | 
                       (0 << DMA_TCD_ATTR_SSIZE_SHIFT)|
                       (0 << DMA_TCD_ATTR_DMOD_SHIFT) | 
                       (0 << DMA_TCD_ATTR_DSIZE_SHIFT);
    
    DMA->TCD[0].NBYTES.MLNO = sizeof(TCD0_Source);    // 每次次循环传输1字节
    DMA->TCD[0].SLAST = -sizeof(TCD0_Source);         // 主循环后恢复源地址（-11字节）

    /*---------------- 目标配置 ----------------*/
    DMA->TCD[0].DADDR = (uint32_t)&TCD0_Dest;    // 目标地址指向单字节变量
    DMA->TCD[0].DOFF = 1;                        // 传输后目标地址不变
    
    /* 当前/初始次循环迭代次数配置 */
    DMA->TCD[0].CITER.ELINKNO = (1 << DMA_TCD_CITER_ELINKNO_CITER_SHIFT) |   // 11次次循环
                                (0 << DMA_TCD_CITER_ELINKNO_ELINK_SHIFT);    // 禁用通道链接
    
    DMA->TCD[0].DLASTSGA = 0;                   // 主循环后不修改目标地址
    
    /*---------------- 控制状态寄存器配置 ----------------*/
    DMA->TCD[0].CSR = (0 << DMA_TCD_CSR_START_SHIFT) |       // 清除START标志
                      (0 << DMA_TCD_CSR_INTMAJOR_SHIFT) |    // 禁用主循环中断
                      (0 << DMA_TCD_CSR_INTHALF_SHIFT) |     // 禁用半循环中断
                      (1 << DMA_TCD_CSR_DREQ_SHIFT) |        // 主循环后禁用通道
                      (0 << DMA_TCD_CSR_ESG_SHIFT) |         // 禁用Scatter-Gather
                      (0 << DMA_TCD_CSR_MAJORELINK_SHIFT) |  // 禁用主循环通道链接
                      (0 << DMA_TCD_CSR_ACTIVE_SHIFT) |      // 清除ACTIVE标志
                      (0 << DMA_TCD_CSR_DONE_SHIFT) |        // 清除DONE标志
                      (0 << DMA_TCD_CSR_MAJORLINKCH_SHIFT) | // 主循环链接通道号=0
                      (0 << DMA_TCD_CSR_BWC_SHIFT);          // 无带宽控制延迟
    
    /* 初始次循环迭代次数配置（与CITER相同） */
    DMA->TCD[0].BITER.ELINKNO = (1 << DMA_TCD_BITER_ELINKNO_BITER_SHIFT) |  // 初始11次迭代
                                (0 << DMA_TCD_BITER_ELINKNO_ELINK_SHIFT);   // 禁用通道链接
}

void Hal_Dma_Init(void) {
    PCC->PCCn[PCC_DMAMUX_INDEX] |= PCC_PCCn_CGC_MASK;  // 使能DMAMUX时钟
    DMAMUX->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;        // 启用通道0
	  Hal_Dma_Tcd_init();
}

//static void DMA_start_transfer(void) {
//    DMA->TCD[0].CSR |= DMA_TCD_CSR_START_MASK;  // 设置START位
//    while ((DMA->TCD[0].CSR & DMA_TCD_CSR_ACTIVE_MASK)); 
//    DMA->TCD[0].CSR |= DMA_TCD_CSR_DONE_MASK;
//}

void Hal_Dma_Start_Function(void)
{
    DMA->SSRT = 0;  // 触发传输    
    while ((DMA->TCD[0].CSR & DMA_TCD_CSR_ACTIVE_MASK));    
    DMA->TCD[0].CSR |= DMA_TCD_CSR_DONE_MASK;  // 清除标志位
}













