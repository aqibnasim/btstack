// hal_cpu.h implementation
#include "hal_cpu.h"
#include "cmsis_gcc.h"
void hal_cpu_disable_irqs(void){
    __disable_irq();
}

void hal_cpu_enable_irqs(void){
    __enable_irq();
}

void hal_cpu_enable_irqs_and_sleep(void){
    __enable_irq();
    __asm__("wfe"); // go to sleep if event flag isn't set. if set, just clear it. IRQs set event flag
}
//void printf_hexdump(const void *data, int size)
//{
//
//}
