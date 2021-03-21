#ifndef __INTERNAL_FLASH_DRIVER_H
#define __INTERNAL_FLASH_DRIVER_H

#include "stdint.h"
#include "stdbool.h"
#include "stm32f4xx_hal.h"

bool FlashWrite(volatile uint32_t *flash, const uint8_t *data, uint32_t count);
bool FlashUnlock();
bool FlashLock();


#endif
