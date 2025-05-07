#include "FreeRTOS.h"
#include "task.h"

//#include "utility.h"

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{


}

uint32_t m_idleCount=0;	
uint32_t m_idleCntFree = 0;
uint16_t m_u16TickCnt = 0;	

#if configUSE_TICK_HOOK	
void vApplicationTickHook( void )
{
	m_u16TickCnt += 1;
	if(m_u16TickCnt >= configTICK_RATE_HZ) // 1s
	{
		m_u16TickCnt = 0;
		m_idleCntFree = m_idleCount;
		m_idleCount = 0;
	}	
}
#endif

uint32_t GetIdleCount(void)
{
	return m_idleCntFree;
}

// get sys time in second 


void vApplicationIdleHook( void )
{
	m_idleCount++;
}

void vApplicationMallocFailedHook( void )
{


}

static  StackType_t   idle_task_stack[128];
StaticTask_t xIdleTaskBuffer;
 /*lint !e526 Symbol not defined as it is an application callback. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, 
									StackType_t **ppxIdleTaskStackBuffer, 
									uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &xIdleTaskBuffer;
	*ppxIdleTaskStackBuffer = idle_task_stack;
	*pulIdleTaskStackSize = 128;
}

static  StackType_t   timer_task_stack[512];
StaticTask_t xTimerTaskBuffer;

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, 
									StackType_t **ppxTimerTaskStackBuffer, 
									uint32_t *pulTimerTaskStackSize )
{
	*ppxTimerTaskTCBBuffer = &xTimerTaskBuffer;
	*ppxTimerTaskStackBuffer = timer_task_stack;
	*pulTimerTaskStackSize = 512;
}
