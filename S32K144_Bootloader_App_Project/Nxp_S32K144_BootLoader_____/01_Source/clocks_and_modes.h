#ifndef CLOCKS_AND_MODES_H_
#define CLOCKS_AND_MODES_H_

#include "include.h" 

void SOSC_init_8MHz (void);
void SPLL_init_160MHz (void);
void NormalRUNmode_80MHz (void);

void SystemCoreClockUpdate(void);
void Test_Delay_Ms(uint32_t ms);

#endif

