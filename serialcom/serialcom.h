/*! \file "serialcom.h"
   \brief Contains functions that establish a communication protocol over UART
*/

#ifndef SERIALCOM_H_
#define SERIALCOM_H_

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "stm32f4xx_hal.h"

#include "peripherals.h"

#include "rtos.h"

#include "btMemOps.h"
#include "btDevCtrl.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CONSTANTS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*!\def POWER_STACK_SIZE
 Size of Stack
*/
#define SERIAL_STACK_SIZE     768

/*!\brief Size of receive buffer
*/
#define SERIAL_RXBUF_SIZE	736//(MEMOPS_PDU_SIZE+3+4)*MEMOPS_DATA_VERIFYSIZE

/*!\brief Size of receive buffer
*/
#define SERIAL_TXBUF_SIZE	32

/*!\var uint32_t SER_HEADER_TIMEOUT
	Number of ticks to wait when waiting for header
*/
static const uint32_t SER_HEADER_TIMEOUT = 0x0FFFFFFF;

/*!\var uint32_t SER_PACKET_TIMEOUT
	Number of ticks to wait when waiting for packet data
*/
static const uint32_t SER_PACKET_TIMEOUT = 5;

/*!\brief Size of memops output buffer
*/
#define SERIAL_PKT_SIZE		MEMOPS_PDU_SIZE+8

/*!\brief Output message delimiters
*/
#define SERIAL_OUT_DELIM1	0x52
#define SERIAL_OUT_DELIM2	0x87
#define SERIAL_OUT_DELIM3	0x49
#define SERIAL_OUT_DELIM4	0x94

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS ------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\typedef SerSemaphore
	Contains semaphore and UART handle. Used to bind semaphore to specified UART peripheral

 \var UART_HandleTypeDef SerSemaphore::sem
 \var uint8_t SerSemaphore::*handle
 Pointer to HAL UART handle
*/
typedef struct
{
	SemaphoreHandle_t sem;
	UART_HandleTypeDef *handle;
} SerSemaphore;

/*!\typedef serialState
	Serial State. Contains necessary peripheral information

 \var UART_HandleTypeDef serialState::*handle
 Pointer to HAL UART handle
 \var char serialState::peripherals
 Pointer to peripheral object
 \var char serialState::rxbuf
 Receive buffer
 \var char serialState::txbuf
 Transmit buffer
*/
typedef struct
{
	UART_HandleTypeDef *handle;

    //Peripherals
    PeriObject *peri;

	char rxbuf[SERIAL_RXBUF_SIZE];
	char procbuf[20];
	char txbuf[SERIAL_TXBUF_SIZE];

	uint16_t rx_index;

    TaskHandle_t task_cmd;
} serialState;

serialState serialcom;


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/


/*! \brief Initialize Serial communication interface

    \param *serial Pointer to Serial object
    \param *handle Uart Handle
    \param *peri Pointer to Peripheral object
*/
void serialcomInit(serialState *serial, UART_HandleTypeDef *handle, PeriObject *peri,
		int priority, size_t stacksize);

/*! \brief Handle IDLE interrupt

    \param *serial Pointer to Serial object
*/
void USER_UART_IDLECallback(UART_HandleTypeDef *huart);


#endif /* SERIALCOM_H_ */
