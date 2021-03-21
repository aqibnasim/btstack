#include "i2s_peri.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/* LOCAL VARIABLES ------------------------------------------------------------*/
uint16_t i2s_buff[I2S_BUFFSZ];

/* GLOBAL VARIABLES ------------------------------------------------------------*/
//Semaphore: Used to indicate a transfer is occuring
I2sObject *i2s_instance[I2S_MAX_PERIPH];

/* STATIC FUNCTIONS -----------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* PUBLIC --------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
//Initialisation
void i2sInit(I2sObject *i2s, I2S_HandleTypeDef *handle, uint8_t periph_index)
{
	i2s->handle = handle;
	i2s->periph_index = periph_index;

	if (i2s->periph_index >= I2S_MAX_PERIPH)
		return;

	i2s->buffer = i2s_buff;

	memset(i2s->buffer, 0, (I2S_BUFFSZ * sizeof(uint16_t)));

	i2s_instance[periph_index] = i2s;
}

/*----------------------------------------------------------------------*/

void i2s_MspInit(I2sObject *i2s)
{
	HAL_I2S_MspInit(i2s);
}

/*----------------------------------------------------------------------*/

void i2s_MspDeInit(I2sObject *i2s)
{
	HAL_I2S_MspDeInit(i2s);
}

/*----------------------------------------------------------------------*/

void i2s_startEmptyStream(I2sObject *i2s)
{
	i2s->playing = false;

	//Start stream
	memset(i2s->buffer, 0, (I2S_BUFFSZ * sizeof(uint16_t)));
	HAL_I2S_Transmit_DMA(i2s->handle, i2s->buffer, I2S_BUFFSZ);

	//Disable interrupts (TC, TE, DME, HT)
	i2s->handle->hdmatx->Instance->CR &= ~(DMA_IT_TC | DMA_IT_TE | DMA_IT_DME | DMA_IT_HT);
}

/*----------------------------------------------------------------------*/

void i2s_stopStream(I2sObject *i2s)
{
	i2s->playing = false;

	HAL_I2S_DMAStop(i2s->handle);

	//Disable interrupts (TC, TE, DME, HT)
	i2s->handle->hdmatx->Instance->CR &= ~(DMA_IT_TC | DMA_IT_TE | DMA_IT_DME | DMA_IT_HT);
}

/*----------------------------------------------------------------------*/

I2sObject *i2s_matchObject(I2S_HandleTypeDef *hi2s)
{
	I2sObject *i2s = NULL;

	for (uint8_t periph_index = 0; periph_index < I2S_MAX_PERIPH; periph_index++)
	{
		if (i2s_instance[periph_index]->handle == hi2s)
		{
			i2s = i2s_instance[periph_index];
			break;
		}
	}

	return i2s;
}

/*----------------------------------------------------------------------*/

bool i2sCheckBusy(I2sObject *i2s)
{
	return i2s->playing;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CALLBACKS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	I2sObject *i2s = i2s_matchObject(hi2s);

	if (i2s == NULL)
		return;
}

/*----------------------------------------------------------------------*/

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	I2sObject *i2s = i2s_matchObject(hi2s);

	if (i2s == NULL)
		return;
}



