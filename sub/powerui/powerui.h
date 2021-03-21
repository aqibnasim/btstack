/*! \file "powerui.h"
   \brief PowerUI header file

    Include for access to global powerui variables and functions
    Functions for PMIC control
    Functions for LED control
    Functions for Pushbutton status
*/

#ifndef POWERUI_H_
#define POWERUI_H_

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

#include "stm32f4xx_hal.h"

#include "rtos.h"
#include "peripherals.h"

#include "powerui_types.h"

#include "powerui_pmic.h"
#include "powerui_led.h"
#include "powerui_pb.h"
#include "powerui_frame.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* SYSTEM CONSTANTS ----------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\def POWER_STACK_SIZE
 Size of Stack
*/
#define POWER_STACK_SIZE     1500

/*!\def POWER_MB_SIZE
 Size of POWERUI Mailbox
*/
#define POWER_MB_SIZE         1

#define PWRUI_I2C_RWSIZE       8
#define PWR_I2C_RETRIES         3

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Initialize PWRUI subsystem

    \param *state Pointer to Powerui object
    \param priority Free-RTOS priority of command thread and timers
    \param *peri Pointer to peripheral object
    \param *pmic_port Pointer to PMIC/LS pin port
    \param cd_pin Index of PMIC CD input
    \param lscntrl_pin Index of PMIC LSCNTRL input
    \param int_pin Index of PMIC PM_INT input
    \param rst_pin Index of PMIC PM_RST input
    \param lsen_pin Index of Load switch enable input
    \param *hall_port Pointer to to Frame hall input port
    \param hall_pin Index of Frame hall input

    \remarks Add additional pins for new schematic
*/
void poweruiInit(PowerUIState *state, int priority, PeriObject *peri,
		GPIO_TypeDef *pmic_port,
		uint16_t cd_pin, uint16_t lscntrl_pin, uint16_t int_pin,
		uint16_t rst_pin, uint16_t lsen_pin,
		GPIO_TypeDef *hall_port, uint16_t hall_pin,
		GPIO_TypeDef *pb_port, uint16_t pb_pin);                      //Initialisations

/*! \brief Post a command into the POWERUI command Queue

    \param *state Pointer to Powerui object
    \param cmd The command issued to Queue
    \param *sem Semaphore used for sequencing
*/
void powerui_post_cmd(PowerUIState *state, PowerUICmdType cmd, SemaphoreHandle_t *sem);

/*! \brief Post a command into the POWERUI command Queue and wait for completion

    \param *state Pointer to Powerui object
    \param *state Pointer to Powerui object
    \param cmd The command issued to Queue
    \param *sem Semaphore used for sequencing
    \param waitcnt Number of ticks to wait before reporting failure
    \return Pass or fail
*/
bool powerui_post_cmdwait(PowerUIState *state, PowerUICmdType cmd, SemaphoreHandle_t *sem, uint32_t waitcnt);

/*! \brief Update PMIC status

    \param *state Pointer to Powerui object

    \remarks Why are we waiting for I2C failure to queue a charge disable? There
    has to be an easier way to do this over I2C
*/
void powerui_bq_update(PowerUIState *state);

/*! \brief Check SOC of Battery

    \param *state Pointer to Powerui object
    \return State of Charge in terms of 0-100%
*/
int8_t poweruiChkSOC(PowerUIState *state);


/* THREADS --------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* POWERUI_H_ */

