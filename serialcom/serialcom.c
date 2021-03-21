/*! \file "serialcom.c"
   \brief Contains functions that establish a communication protocol over UART
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "serialcom.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* LOCAL VARIABLES -----------------------------------------------------*/
/*----------------------------------------------------------------------*/
serialState *ser;

/*!\var SerSemaphore ser_sem_done
	Used to signal end of operation in Blocking (Wait) TX/RX operations
*/
SerSemaphore ser_sem_done;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*! \brief Initialize Serial communication interface

    \param *state Pointer to Audio object
*/
static void serialcomIncoming(void *arg);

/*! \brief Return result of last transfer

    \param *serial Pointer to serialState
    \return Pass or fail
*/
static bool serTransferResult(serialState *serial);

/*! \brief Wait for completion of last execution Read/Write

    \param *serial Pointer to serialState
    \param timeout Number of ticks to wait
    \return Pass or fail
*/
static bool serWaitComplete(serialState *serial, uint32_t timeout);

/*! \brief Release semaphore

	\param *huart Pointer to HAL UART handle
*/
static void serReleaseLock(UART_HandleTypeDef *huart);

/*! \brief Increment the RX buffer index

	\param *serial Pointer to serialState
	\param inc Increment by this amount
*/
static uint16_t serRXIncIndex(serialState *serial, uint16_t inc);

/*! \brief Set a parameter over UART

    \param param Profile parameter ID
    \param len length of data to write
    \param *value pointer to data to write.  This is dependent on
             the parameter ID and may be cast to the appropriate
             data type (example: data type of uint16_t will be cast to
             uint16_t pointer).

    \return Success
*/
static bool service_SetParameter_UART(uint8_t param,
                                          uint16_t len,
                                          void *value);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INITIALIZATION ------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void serialcomInit(serialState *serial, UART_HandleTypeDef *handle, PeriObject *peri,
		int priority, size_t stacksize)
{
#ifndef UART_LOAD
	return;
#endif

	ser = serial;

	serial->handle = handle;
	serial->peri = peri;

	serial->rx_index = 0;

	rtos_createSemaphore(&ser_sem_done.sem, 255, 0);
	ser_sem_done.handle = handle;

	rtos_createThread(&serial->task_cmd, priority, stacksize,
			serialcomIncoming, "Serial Processing Task", (void *) serial);
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* PROCESSING ----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void serialcomIncoming(void *arg)
{
	serialState *serial = (serialState *)arg;

	HAL_UART_Receive_DMA(serial->handle, serial->rxbuf, SERIAL_RXBUF_SIZE);

	while (1)
	{
		serWaitComplete(serial, SER_HEADER_TIMEOUT);
		HAL_UART_Receive_DMA(serial->handle, serial->rxbuf, SERIAL_RXBUF_SIZE);

		serial->rx_index = 0;

		uint16_t service_uuid = serial->rxbuf[serial->rx_index];
		service_uuid += serial->rxbuf[serRXIncIndex(serial, 1)] << 8;
		uint8_t char_uuid = serial->rxbuf[serRXIncIndex(serial, 1)];
		uint16_t len = serial->rxbuf[serRXIncIndex(serial, 1)];
		len += (serial->rxbuf[serRXIncIndex(serial, 1)] << 8);
		uint16_t packet_index = 0;
		uint16_t packet_len = 0;

		if (service_uuid == MEMOPS_SERV_UUID)
		{
			while (packet_index < len)
			{
				if ((packet_index + MEMOPS_PDU_SIZE + 3) >= len)
				{
					packet_len = len - packet_index;
				}
				else
				{
					packet_len = MEMOPS_PDU_SIZE + 3;
				}

				//Copy packet into buffer
				for (uint16_t i = 0; i < packet_len; i++)
				{
					serial->procbuf[i] = serial->rxbuf[serRXIncIndex(serial, 1)];
				}

				btMemOpsService_Handler(MEMOPS_UART, &serial->peri->mem,
											serial->procbuf, char_uuid);

				packet_index += packet_len;
			}
		}
		else if (service_uuid == DEVCTRL_SERV_UUID)
		{
			while (packet_index < len)
			{
				if ((packet_index + DEVCTRL_PDU_SIZE + 3) >= len)
				{
					packet_len = len - packet_index;
				}
				else
				{
					packet_len = DEVCTRL_PDU_SIZE + 3;
				}

				//Copy packet into buffer
				for (uint16_t i = 0; i < packet_len; i++)
				{
					serial->procbuf[i] = serial->rxbuf[serRXIncIndex(serial, 1)];
				}

				btDevCtrlService_Handler(DEVCTRL_UART, &main_state,
											serial->procbuf, char_uuid);

				packet_index += packet_len;
			}
		}

		serRXIncIndex(serial, 1);
	}
}

/*----------------------------------------------------------------------*/

bool serWaitComplete(serialState *serial, uint32_t timeout)
{
	if (xSemaphoreTake(ser_sem_done.sem, timeout) == pdFALSE)
		return false;

	return serTransferResult(serial);
}

/*----------------------------------------------------------------------*/

bool serTransferResult(serialState *serial)
{
    //Return result
    if (serial->handle->ErrorCode != HAL_UART_ERROR_NONE)
    	return false;
    else
    	return true;
}

/*----------------------------------------------------------------------*/

void serReleaseLock(UART_HandleTypeDef *huart)
{
	static BaseType_t xHigherPriorityTaskWoken;

	if (ser_sem_done.handle == huart)
	{
		xSemaphoreGiveFromISR(ser_sem_done.sem, &xHigherPriorityTaskWoken);
	}

	if (xHigherPriorityTaskWoken == pdTRUE)
	{
		//Do nothing for now
	}
}

/*----------------------------------------------------------------------*/

uint16_t serRXIncIndex(serialState *serial, uint16_t inc)
{
	serial->rx_index += inc;
	if (serial->rx_index >= SERIAL_RXBUF_SIZE)
		serial->rx_index -= SERIAL_RXBUF_SIZE;

	return serial->rx_index;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* TRANSMIT BACK -------------------------------------------------------*/
/*----------------------------------------------------------------------*/

bool MemOpsService_SetParameter_UART(uint8_t param,
                                          uint16_t len,
                                          void *value)
{
	service_SetParameter_UART(param, len, value);
}

bool DevCtrlService_SetParameter_UART(uint8_t param,
                                          uint16_t len,
                                          void *value)
{
	service_SetParameter_UART(param, len, value);
}

bool service_SetParameter_UART(uint8_t param,
        uint16_t len,
        void *value)
{
	char *outbuf = ser->txbuf;
	char *out = (char *)value;

	for (uint16_t index = 0; index < len; index++)
	{
		outbuf[index] = out[index];
	}

	outbuf[len] = SERIAL_OUT_DELIM1;
	outbuf[len+1] = SERIAL_OUT_DELIM2;
	outbuf[len+2] = SERIAL_OUT_DELIM3;
	outbuf[len+3] = SERIAL_OUT_DELIM4;

	HAL_UART_Transmit_DMA(ser->handle, outbuf, len+4);
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CALLBACKS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	//serReleaseLock(huart);
}

/*----------------------------------------------------------------------*/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//serReleaseLock(huart);
}

/*----------------------------------------------------------------------*/

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	//serReleaseLock(huart);
}

/*----------------------------------------------------------------------*/

void USER_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	//Stop this DMA transmission
	HAL_UART_DMAStop(huart);
	serReleaseLock(huart);

}
