#ifndef INCLUDE_H_
#define INCLUDE_H_

#define  Bsp_GPio_HighPin(var,bit)	  (var ->PDOR|= ((0x01)<<bit))
#define  Bsp_Gpio_LowPin(var,bit)     (var ->PDOR&= (~(0x01<<bit)))
#define  Bsp_Gpio_TogglePin(var,bit)  (var ->PTOR ^=(0x01<<bit))

#define  BSP_SYSYEM_SET_INT_PRIO(x)	((x << 4) & 0xff)
#define  BSP_CAN_INT_PRIO	(5 + 1) // 0x06


#include <string.h>
#include "status.h"

#include "device_registers.h"           // Device header
#include "clocks_and_modes.h"
#include "interrupt_manager.h"

#include "flash_driver.h"
#include "hal_flash.h"
#include "hal_clocks_and_modes.h"
#include "hal_can.h"

#include "uds_service.h"
#include "uds_tp.h"
#endif
