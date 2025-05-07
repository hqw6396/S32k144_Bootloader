#ifndef CLOCKS_AND_MODES_H_
#define CLOCKS_AND_MODES_H_

#include "include.h"   

void Bsp_System_Clock_Init(void);
void Bsp_Wdog_Disable_Init(void);
void SystemCoreClockUpdate(void);
void Bsp_Delay_Ms(uint32_t ms);

#endif /* CLOCKS_AND_MODES_H_ */

