/*! \file "i2c_peri.h"
   \brief I2C Abstraction layer.
   Provides functions to interface with HAL drivers
*/

#ifndef I2C_H_
#define I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

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

/*!\def I2C_MAX_PERIPH
	Maximum number of I2C peripherals allowed
*/
#define I2C_MAX_PERIPH		1

/*!\var uint32_t I2C_TRANSFER_TIMEOUT
	Number of ticks to wait before a transfer timeouts
*/
static const uint32_t I2C_TRANSFER_TIMEOUT = portMAX_DELAY;

static const uint32_t I2C_NOADDR = 0xFFFFFFFF;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS ------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\typedef I2cTransaction
	Transaction Object contains relevant information for a given connection
	(address, buffer locations etc).

 \var uint8_t I2cTransaction::slave_address
 I2C slave address
 \var size_t I2cTransaction::mem_addr_size
 Size of slave memory address in bytes
 \var void I2cTransaction::*read_buff
 Pointer to our Receive buffer
 \var void I2cTransaction::*write_buff
 Pointer to our Transmit buffer
 \var uint32_t I2cTransaction::mem_addr
 Internal memory address
 \var size_t I2cTransaction::write_count
 Number of bytes to write
 \var size_t I2cTransaction::read_count
 Number of bytes to read
*/
typedef struct
{
	//Initialized parameters
	uint8_t slave_address;
	size_t mem_addr_size;
	void *read_buff;
	void *write_buff;

	//Modified for each transfer
	uint32_t mem_addr;
	size_t write_count;
	size_t read_count;
} I2cTransaction;

/*!\typedef I2cObject
	I2C Object. Contains necessary peripheral information

 \var I2C_HandleTypeDef I2cObject::*handle
 Pointer to HAL I2C handle
*/
typedef struct
{
    //I2C objects
    I2C_HandleTypeDef *handle;

    bool busy;
} I2cObject;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Initialize I2cObject

    \param *i2c Pointer to I2cObject object
    \param *handle Pointer to HAL I2C handle
*/
void i2cInit(I2cObject *i2c, I2C_HandleTypeDef *handle);

/*! \brief Start low level hardware for I2C

    \param *i2c Pointer to I2cObject object
*/
void i2c_MspInit(I2cObject *i2c);

/*! \brief Reset low level hardware for I2C

    \param *i2c Pointer to I2cObject object
*/
void i2c_MspDeInit(I2cObject *i2c);

/*! \brief Initialize I2C Transaction object

    \param *new_trans Pointer to Transaction object
    \param address Slave Address
    \param mem_addr_size Size of slave memory address in bytes
    \param *read_buff Pointer to Receive buffer
    \param *write_buff Pointer to Transmit buffer
*/
void i2cInitTransaction(I2cTransaction *new_trans, uint8_t address, size_t mem_addr_size,
                        void *read_buff, void *write_buff);

/*! \brief Start an I2C Write and wait for completion

    \param *i2c Pointer to I2cObject
    \param *new_trans Pointer to transaction object
    \param mem_addr Slave internal memory address
	\param writeCount Number of bytes to write
    \return Pass or fail
*/
bool i2cNewTXWait(I2cObject *i2c, I2cTransaction *new_trans,
                    uint32_t mem_addr, uint8_t writeCount);

/*! \brief Start an I2C Read and wait for completion

    \param *i2c Pointer to I2cObject
    \param *new_trans Pointer to transaction object
    \param mem_addr Slave internal memory address
	\param readCount Number of bytes to read
    \return Pass or fail
*/
bool i2cNewRXWait(I2cObject *i2c, I2cTransaction *new_trans,
					uint32_t mem_addr, uint8_t readCount);

/*! \brief Wait for completion of last execution Read/Write

    \param *i2c Pointer to I2cObject
    \return Pass or fail
*/
bool i2cWaitComplete(I2cObject *i2c);

/*! \brief Check if peripheral is busy

    \param *i2c Pointer to I2cObject
    \return True: Busy. False: Idle.
*/
bool i2cCheckBusy(I2cObject *i2c);

#ifdef __cplusplus
}
#endif


#endif /* I2C_H_ */
