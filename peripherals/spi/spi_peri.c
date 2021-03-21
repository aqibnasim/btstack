/*! \file "spi_peri.c"
   \brief SPI Abstraction layer.
   Provides functions to interface with HAL drivers

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "spi_peri.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* LOCAL VARIABLES -----------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\var SemaphoreHandle_t spi_sem_lock
	Locks up I2C access until last operation completed
*/
SpiSemaphore spi_sem_lock[SPI_MAX_PERIPH];

/*!\var SemaphoreHandle_t spi_sem_done
	Used to signal end of operation in Blocking (Wait) TX/RX operations
*/
SpiSemaphore spi_sem_done[SPI_MAX_PERIPH];

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Start an SPI Transfer with SPI peripheral pointed to by *spi, and
 	settings in *trans.

    \param *spi Pointer to SpiObject
    \param *trans Pointer to SPI Transaction object
    \return Pass or fail
*/
static bool spiTransfer(SpiObject *spi, SpiTransaction *trans);

/*! \brief Return result of last transfer

    \param *spi Pointer to SpiObject
    \return Pass or fail
*/
static bool spiTransferResult(SpiObject *spi);

/*! \brief Release both Semaphores (spi_sem_lock and spi_sem_done)

	\param *hspi Pointer to HAL SPI handle
*/
static void spiReleaseLock(SPI_HandleTypeDef *hspi);

/*! \brief Start an SPI Transfer (non-blocking)

    \param *spi Pointer to SpiObject
    \param *new_trans Pointer to transaction object
	\param count Size of transaction
	\param mode Transfer mode
    \return Pass or fail
*/
static bool spiNewTransfer(SpiObject *spi, SpiTransaction *new_trans,
		size_t count, SpiTransferMode mode);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void spiInit(SpiObject *spi, SPI_HandleTypeDef *handle, uint8_t periph_index)
{
	spi->handle = handle;
	spi->periph_index = periph_index;

	if (spi->periph_index >= SPI_MAX_PERIPH)
		return;

    //Resource control
    rtos_createSemaphore(&spi_sem_lock[spi->periph_index].sem, 1, 1);
    rtos_createSemaphore(&spi_sem_done[spi->periph_index].sem, 1, 0);

    spi_sem_lock[spi->periph_index].handle = handle;
    spi_sem_done[spi->periph_index].handle = handle;
}

/*----------------------------------------------------------------------*/

void spi_MspInit(SpiObject *spi)
{
	HAL_SPI_MspInit(spi);
}

/*----------------------------------------------------------------------*/

void spi_MspDeInit(SpiObject *spi)
{
	HAL_SPI_MspDeInit(spi);
}

/*----------------------------------------------------------------------*/

void spiInitTransaction(SpiTransaction *new_trans,
		void *read_buff, void *write_buff)
{
	new_trans->read_buff = read_buff;
	new_trans->write_buff = write_buff;
}

/*----------------------------------------------------------------------*/

bool spiNewTransferWait(SpiObject *spi, SpiTransaction *new_trans, size_t count, SpiTransferMode mode)
{
    if (!spiNewTransfer(spi, new_trans, count, mode))
    	return false;

    bool result = spiWaitComplete(spi);

    return result;
}

/*----------------------------------------------------------------------*/

bool spiNewTransfer(SpiObject *spi, SpiTransaction *new_trans, size_t count, SpiTransferMode mode)
{
	new_trans->count = count;
	new_trans->mode = mode;

	return spiTransfer(spi, new_trans);
}

/*----------------------------------------------------------------------*/

bool spiWaitComplete(SpiObject *spi)
{
	if (xSemaphoreTake(spi_sem_done[spi->periph_index].sem, SPI_TRANSFER_TIMEOUT) == pdFALSE)
		return false;

	spi->busy = false;

	return spiTransferResult(spi);
}

/*----------------------------------------------------------------------*/

bool spiCheckBusy(SpiObject *spi)
{
	return spi->busy;
}

/*----------------------------------------------------------------------*/

bool spiTransferResult(SpiObject *spi)
{
    //Return result
    if (spi->handle->ErrorCode != HAL_SPI_ERROR_NONE)
    	return false;
    else
    	return true;
}

/*----------------------------------------------------------------------*/

bool spiTransfer(SpiObject *spi, SpiTransaction *trans)
{
	//Flag busy state
	spi->busy = true;

	//Wait for spi to be freed up
	if (xSemaphoreTake(spi_sem_lock[spi->periph_index].sem, SPI_TRANSFER_TIMEOUT) == pdFALSE)
		return false;

	switch (trans->mode)
	{
	case SPI_READ:
		HAL_SPI_Receive_IT(spi->handle, trans->read_buff, trans->count);
		break;
	case SPI_WRITE:
		HAL_SPI_Transmit_IT(spi->handle, trans->write_buff, trans->count);
		break;
	case SPI_RW:
		HAL_SPI_TransmitReceive_IT(spi->handle, trans->write_buff, trans->read_buff, trans->count);
		break;
	}

    return true;
}

/*----------------------------------------------------------------------*/

void spiReleaseLock(SPI_HandleTypeDef *hspi)
{
	static BaseType_t xHigherPriorityTaskWoken;

	uint8_t periph_index;

	for (periph_index = 0; periph_index < SPI_MAX_PERIPH; periph_index++)
	{
		if (spi_sem_lock[periph_index].handle == hspi)
		{
			xSemaphoreGiveFromISR(spi_sem_lock[periph_index].sem, &xHigherPriorityTaskWoken);
			xSemaphoreGiveFromISR(spi_sem_done[periph_index].sem, &xHigherPriorityTaskWoken);
			break;
		}
	}

	if (xHigherPriorityTaskWoken == pdTRUE)
	{
		//Do nothing for now
	}
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CALLBACKS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	spiReleaseLock(hspi);
}

/*----------------------------------------------------------------------*/

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	spiReleaseLock(hspi);
}

/*----------------------------------------------------------------------*/

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	spiReleaseLock(hspi);
}

/*----------------------------------------------------------------------*/

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	spiReleaseLock(hspi);
}

/*----------------------------------------------------------------------*/

void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi)
{
	spiReleaseLock(hspi);
}

