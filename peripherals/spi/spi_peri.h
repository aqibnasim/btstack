/*! \file "spi_peri.h"
   \brief SPI Abstraction layer.
   Provides functions to interface with HAL drivers
*/

#ifndef SPI_H_
#define SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

//#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Driver Header files */
#include "stm32f4xx_hal.h"

#include "rtos.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CONSTANTS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\def SPI_MAX_PERIPH
	Maximum number of SPI peripherals allowed
*/
#define SPI_MAX_PERIPH		2

/*!\var uint32_t SPI_TRANSFER_TIMEOUT
	Number of ticks to wait before a transfer timeouts
*/
static const uint32_t SPI_TRANSFER_TIMEOUT = 5000;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS ------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\enum SpiTransferMode
	Types of SPI Transfers
*/
typedef enum
{
	SPI_READ = 0,
	SPI_WRITE,
	SPI_RW
} SpiTransferMode;

/*!\typedef SpiTransaction
	Transaction Object contains relevant information for a given connection

 \var void SpiTransaction::*read_buff
 Pointer to our Receive buffer
 \var void SpiTransaction::*write_buff
 Pointer to our Transmit buffer
 \var size_t SpiTransaction::count
 Number of bytes to be read/write in transfer
 \var SpiTransferMode SpiTransaction::mode
 Transfer mode
*/
typedef struct
{
	//Initialized parameters
	void *read_buff;
	void *write_buff;

	//Modified for each transfer
    size_t count;
    SpiTransferMode mode;
}SpiTransaction;

/*!\typedef SpiObject
	SPI Object. Contains necessary peripheral information

 \var SPI_HandleTypeDef SpiObject::*handle
 Pointer to HAL SPI handle
 \var uint8_t SpiObject::periph_index
 Index of SPI peripheral
 \var bool SpiObject::busy
 Is peripheral busy
*/
typedef struct
{
    //SPI objects
	SPI_HandleTypeDef *handle;
	uint8_t periph_index;
	bool busy;
} SpiObject;

/*!\typedef SpiSemaphore
	Contains semaphore and SPI handle. Used to bind semaphore to specified SPI peripheral

 \var SPI_HandleTypeDef SpiSemaphore::sem
 \var uint8_t SpiSemaphore::*handle
 Pointer to HAL SPI handle
*/
typedef struct
{
	SemaphoreHandle_t sem;
	SPI_HandleTypeDef *handle;
} SpiSemaphore;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Initialize SpiObject

    \param *spi Pointer to SpiObject object
    \param *handle Pointer to HAL SPI handle
    \param periph_index Index of SPI peripheral
*/
void spiInit(SpiObject *spi, SPI_HandleTypeDef *handle, uint8_t periph_index);

/*! \brief Start low level hardware for SPI

    \param *spi Pointer to SpiObject object
*/
void spi_MspInit(SpiObject *spi);

/*! \brief Reset low level hardware for SPI

    \param *spi Pointer to SpiObject object
*/
void spi_MspDeInit(SpiObject *spi);

/*! \brief Initialize SPI Transaction object

    \param *new_trans Pointer to Transaction object
    \param *read_buff Pointer to Receive buffer
    \param *write_buff Pointer to Transmit buffer
*/
void spiInitTransaction(SpiTransaction *new_trans,
                        void *read_buff, void *write_buff);

/*! \brief Start an SPI Transfer and wait for completion

    \param *spi Pointer to SpiObject
    \param *new_trans Pointer to transaction object
	\param count Size of transaction
	\param mode Transfer mode
    \return Pass or fail
*/
bool spiNewTransferWait(SpiObject *spi, SpiTransaction *new_trans,
		size_t count, SpiTransferMode mode);

/*! \brief Wait for completion of last execution Read/Write

    \param *spi Pointer to SpiObject
    \return Pass or fail
*/
bool spiWaitComplete(SpiObject *spi);

/*! \brief Check if peripheral is busy

    \param *spi Pointer to SpiObject
    \return True: Busy. False: Idle.
*/
bool spiCheckBusy(SpiObject *spi);

/* THREADS --------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif


#endif /* SPI_H_ */
