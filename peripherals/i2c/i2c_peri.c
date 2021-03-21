/*! \file "i2c_peri.c"
   \brief I2C Abstraction layer.
   Provides functions to interface with HAL drivers
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "i2c_peri.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* LOCAL VARIABLES -----------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\var SemaphoreHandle_t i2c_sem_lock
	Locks up I2C access until last operation completed
*/
SemaphoreHandle_t i2c_sem_lock;

/*!\var SemaphoreHandle_t i2c_sem_done
	Used to signal end of operation in Blocking (Wait) TX/RX operations
*/
SemaphoreHandle_t i2c_sem_done;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Start an I2C Transfer with I2C peripheral pointed to by *i2c, and
 	settings in *trans.

    \param *i2c Pointer to I2cObject
    \param *trans Pointer to I2C Transaction object
    \return Pass or fail
*/
static bool i2cTransfer(I2cObject *i2c, I2cTransaction *trans);

/*! \brief Return result of last transfer

    \param *i2c Pointer to I2cObject
    \return Pass or fail
*/
static bool i2cTransferResult(I2cObject *i2c);

/*! \brief Release both Semaphores (i2c_sem_lock and i2c_sem_done)
*/
static void i2cReleaseLock(void);

/*! \brief Start an I2C Write

    \param *i2c Pointer to I2cObject
    \param *new_trans Pointer to transaction object
    \param mem_addr Slave internal memory address
	\param writeCount Number of bytes to write
    \return Pass or fail
*/
static bool i2cNewTX(I2cObject *i2c, I2cTransaction *new_trans,
                    uint32_t mem_addr, uint8_t writeCount);

/*! \brief Start an I2C Read

    \param *i2c Pointer to I2cObject
    \param *new_trans Pointer to transaction object
    \param mem_addr Slave internal memory address
	\param readCount Number of bytes to read
    \return Pass or fail
*/
static bool i2cNewRX(I2cObject *i2c, I2cTransaction *new_trans,
					uint32_t mem_addr, uint8_t readCount);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void i2cInit(I2cObject *i2c, I2C_HandleTypeDef *handle)
{
	i2c->handle = handle;

    //Resource control
    rtos_createSemaphore(&i2c_sem_lock, 1, 1);
    rtos_createSemaphore(&i2c_sem_done, 1, 0);
}

/*----------------------------------------------------------------------*/

void i2c_MspInit(I2cObject *i2c)
{
	HAL_I2C_MspInit(i2c);
}

/*----------------------------------------------------------------------*/

void i2c_MspDeInit(I2cObject *i2c)
{
	HAL_I2C_MspDeInit(i2c);
}

/*----------------------------------------------------------------------*/

void i2cInitTransaction(I2cTransaction *new_trans, uint8_t address, size_t mem_addr_size,
                        void *read_buff, void *write_buff)
{
    new_trans->slave_address = address << 1;
    new_trans->mem_addr_size = mem_addr_size;
    new_trans->read_buff = read_buff;
    new_trans->write_buff = write_buff;
}

/*----------------------------------------------------------------------*/

bool i2cNewTXWait(I2cObject *i2c, I2cTransaction *new_trans,
                    uint32_t mem_addr, uint8_t writeCount)
{
	if (!i2cNewTX(i2c, new_trans, mem_addr, writeCount))
		return false;

	bool result = i2cWaitComplete(i2c);

	return result;
}

/*----------------------------------------------------------------------*/

bool i2cNewRXWait(I2cObject *i2c, I2cTransaction *new_trans,
					uint32_t mem_addr, uint8_t readCount)
{
	if (!i2cNewRX(i2c, new_trans, mem_addr, readCount))
		return false;

	bool result = i2cWaitComplete(i2c);

	return result;
}

/*----------------------------------------------------------------------*/

bool i2cNewTX(I2cObject *i2c, I2cTransaction *new_trans,
                    uint32_t mem_addr, uint8_t writeCount)
{
	new_trans->mem_addr = mem_addr;
	new_trans->write_count = writeCount;
	new_trans->read_count = 0;

	bool result = i2cTransfer(i2c, new_trans);

	return result;
}

/*----------------------------------------------------------------------*/

bool i2cNewRX(I2cObject *i2c, I2cTransaction *new_trans,
					uint32_t mem_addr, uint8_t readCount)
{
	new_trans->mem_addr = mem_addr;
	new_trans->write_count = 0;
	new_trans->read_count = readCount;

	bool result = i2cTransfer(i2c, new_trans);

	return result;
}

/*----------------------------------------------------------------------*/

bool i2cWaitComplete(I2cObject *i2c)
{
	while(!xSemaphoreTake(i2c_sem_done, I2C_TRANSFER_TIMEOUT));

	i2c->busy = false;

	return i2cTransferResult(i2c);
}

/*----------------------------------------------------------------------*/

bool i2cCheckBusy(I2cObject *i2c)
{
	return i2c->busy;
}

/*----------------------------------------------------------------------*/

bool i2cTransferResult(I2cObject *i2c)
{
    //Return result
    if (i2c->handle->ErrorCode != HAL_I2C_ERROR_NONE)
    	return false;
    else
    	return true;
}

/*----------------------------------------------------------------------*/

bool i2cTransfer(I2cObject *i2c, I2cTransaction *trans)
{
	//Flag busy state
	i2c->busy = true;

    //Wait for i2c to be freed up
    while(!xSemaphoreTake(i2c_sem_lock, I2C_TRANSFER_TIMEOUT));

    //Execute transaction (non-blocking)
    if (trans->read_count > 0)
    {
    	if (trans->mem_addr == I2C_NOADDR)
		{
			HAL_I2C_Master_Receive_IT(i2c->handle, trans->slave_address,
					trans->read_buff, trans->read_count);
		}
    	else
    	{
    		HAL_I2C_Mem_Read_IT(i2c->handle, trans->slave_address,
					trans->mem_addr, trans->mem_addr_size,
					trans->read_buff, trans->read_count);
    	}
    }
    else if (trans->write_count > 0)
    {
    	if (trans->mem_addr == I2C_NOADDR)
    	{
    		HAL_I2C_Master_Transmit_IT(i2c->handle, trans->slave_address,
					trans->write_buff, trans->write_count);
    	}
    	else
    	{
			HAL_I2C_Mem_Write_IT(i2c->handle, trans->slave_address,
					trans->mem_addr, trans->mem_addr_size,
					trans->write_buff, trans->write_count);;
    	}
    }
    else
    	return false;

    return true;
}

/*----------------------------------------------------------------------*/

void i2cReleaseLock(void)
{
	static BaseType_t xHigherPriorityTaskWoken;

	xSemaphoreGiveFromISR(i2c_sem_lock, &xHigherPriorityTaskWoken);
	xSemaphoreGiveFromISR(i2c_sem_done, &xHigherPriorityTaskWoken);

	if (xHigherPriorityTaskWoken == pdTRUE)
	{
		//
	}
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CALLBACKS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	i2cReleaseLock();
}

/*----------------------------------------------------------------------*/

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	i2cReleaseLock();
}

/*----------------------------------------------------------------------*/

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	i2cReleaseLock();
}

/*----------------------------------------------------------------------*/

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	i2cReleaseLock();
}

/*----------------------------------------------------------------------*/

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	i2cReleaseLock();
}

