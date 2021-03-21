/*! \file "peripherals.h"
   \brief Contains all peripherals in a single object. Include to gain access
   to global functions/declarations for all peripherals.*/

#ifndef PERIPHERALS_PERIPHERALS_H_
#define PERIPHERALS_PERIPHERALS_H_

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/* Peripheral headers */
#include "i2c/i2c_peri.h"
#include "spi/spi_peri.h"
#include "i2s/i2s_peri.h"
#include "memory/mem_peri.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS ------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\typedef PeriObject
	Contains all peripheral objects.

 \var I2cObject PeriObject::i2c
 \var SpiObject PeriObject::spi
 \var I2sObject PeriObject::i2s
 \var MemObject PeriObject::mem
*/
typedef struct
{
    I2cObject i2c;
    SpiObject spi;
    I2sObject i2s;
    MemObject mem;
    MemObject mem_aud;
} PeriObject;



#endif /* PERIPHERALS_PERIPHERALS_H_ */
