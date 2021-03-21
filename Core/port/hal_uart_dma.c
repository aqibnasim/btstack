// hal_uart_dma.c implementation
#include "hal_uart_dma.h"
#include "main.h"
static void dummy_handler(void);


// handlers
static void (*rx_done_handler)(void) = &dummy_handler;
static void (*tx_done_handler)(void) = &dummy_handler;

static void dummy_handler(void){};

void hal_uart_dma_set_sleep(uint8_t sleep){
    // later..
}

// reset Bluetooth using nShutdown
static void bluetooth_power_cycle(void){
    HAL_GPIO_WritePin( GPIOA, GPIO_PIN_8, GPIO_PIN_RESET );
    HAL_Delay( 250 );
    HAL_GPIO_WritePin( GPIOA, GPIO_PIN_8, GPIO_PIN_SET );
    HAL_Delay( 500 );
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
    if (huart == &huart1){
        (*tx_done_handler)();
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if (huart == &huart1){
        (*rx_done_handler)();
    }
}

void hal_uart_dma_init(void){
    bluetooth_power_cycle();
}
void hal_uart_dma_set_block_received( void (*the_block_handler)(void)){
    rx_done_handler = the_block_handler;
}

void hal_uart_dma_set_block_sent( void (*the_block_handler)(void)){
    tx_done_handler = the_block_handler;
}

void hal_uart_dma_set_csr_irq_handler( void (*the_irq_handler)(void)){
    // .. later
}

int  hal_uart_dma_set_baud(uint32_t baud){
    // .. later
    return 0;
}

void hal_uart_dma_send_block(const uint8_t *data, uint16_t size){
    HAL_UART_Transmit_DMA( &huart1, (uint8_t *) data, size);
}

void hal_uart_dma_receive_block(uint8_t *data, uint16_t size){
    HAL_UART_Receive_DMA( &huart1, data, size );
}

