#include "hal_time_ms.h"
uint32_t hal_time_ms(void){
    return HAL_GetTick();
}
