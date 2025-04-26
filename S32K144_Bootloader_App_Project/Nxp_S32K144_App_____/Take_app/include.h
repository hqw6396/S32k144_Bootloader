#ifndef INCLUDE_H_
#define INCLUDE_H_

#define  Bsp_GPio_HighPin(var,bit)	  (var ->PDOR|= ((0x01)<<bit))
#define  Bsp_Gpio_LowPin(var,bit)     (var ->PDOR&= (~(0x01<<bit)))
#define  Bsp_Gpio_TogglePin(var,bit)  (var ->PTOR ^=(0x01<<bit))

#include "device_registers.h"           // Device header
#include "clocks_and_modes.h"
        
#include "FreeRTOS.h"
#include "task.h"

#endif
