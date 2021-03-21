/*! \file "i2s_peri.h"
   \brief I2S Abstraction layer.
   Provides functions to interface with HAL drivers
*/

#ifndef I2S_H_
#define I2S_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/* Driver Header files */
#include "stm32f4xx_hal.h"

#include "memory/mem_peri.h"

#include "rtos.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CONSTANTS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\def I2S_MAX_PERIPH
	Maximum number of I2S peripherals
*/
#define I2S_MAX_PERIPH		1

/*!\var uint32_t I2S_TRANSFER_TIMEOUT
	Number of ticks to wait before a transfer timeouts
*/
static const uint32_t I2S_TRANSFER_TIMEOUT = 5000;

/*!\def I2S_BUFFSZ
	Size of output buffer in 16bit words
*/
#define I2S_BUFFSZ	1024

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS ------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\typedef I2sCmd
	I2S Object. Contains necessary peripheral information

 \var MemChunk I2sCmd::*audfile_chunk
 Pointer to Audio Clip MemChunk object
  \var uint16_t I2sCmd::audfile_index
 Element index of audio clip within memchunk
*/
typedef struct
{
	MemChunk *audfile_chunk;
	uint16_t audfile_index;
} I2sCmd;

/*!\typedef I2sObject
	I2S Object. Contains necessary peripheral information

 \var I2S_HandleTypeDef I2sObject::*handle
 Pointer to HAL I2S handle
  \var uint8_t I2sObject::periph_index
 I2S peripheral index
  \var uint16_t I2sObject::buffer
 Buffer for I2S output
  \var bool I2sObject::playing
 Flag indicates if currently outputting on I2S
*/
typedef struct
{
    //I2S objects
	I2S_HandleTypeDef *handle;
	uint8_t periph_index;
	uint16_t *buffer;

	//Object control
	bool playing;
} I2sObject;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Initialize I2sObject

    \param *i2s Pointer to I2sObject object
    \param *handle Pointer to HAL I2S handle
    \param periph_index Index of I2S peripheral
*/
void i2sInit(I2sObject *i2s, I2S_HandleTypeDef *handle, uint8_t periph_index);

/*! \brief Start low level hardware for I2S

    \param *i2s Pointer to I2sObject object
*/
void i2s_MspInit(I2sObject *i2s);

/*! \brief Reset low level hardware for I2S

    \param *i2s Pointer to I2sObject object
*/
void i2s_MspDeInit(I2sObject *i2s);

/*! \brief Start an empty I2S stream (silent)

    \param *i2s Pointer to I2sObject object
*/
void i2s_startEmptyStream(I2sObject *i2s);

/*! \brief Stop an I2S stream

    \param *i2s Pointer to I2sObject object
*/
void i2s_stopStream(I2sObject *i2s);

/*! \brief Check if peripheral is busy

    \param *i2s Pointer to I2sObject
    \return True: Busy. False: Idle.
*/
bool i2sCheckBusy(I2sObject *i2s);

/* THREADS --------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif


#endif /* I2C_H_ */
