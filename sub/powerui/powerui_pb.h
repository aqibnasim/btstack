/*! \file "powerui_pb.h"
   \brief PowerUI PB subsystem file
*/

#ifndef POWERUI_PB_H_
#define POWERUI_PB_H_

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

#include "powerui_types.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* PUSHBUTTON CONSTANTS ------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\var uint32_t PB_PRESSENABLE_PERIOD
	Amount of time to debounce PB
*/
static const uint32_t PB_PRESSENABLE_PERIOD = 50;

/*!\var uint32_t PB_3SPRESS_PERIOD
	Amount of time PB to be pressed to register LongPress_3s
*/
static const uint32_t PB_3SPRESS_PERIOD = 3000;

/*!\var uint32_t PB_3t6SPRESS_PERIOD
	Amount of additional time PB to be pressed to register LongPress_6s
*/
static const uint32_t PB_3t6SPRESS_PERIOD = 3000;

/*!\var uint32_t PB_6t10SPRESS_PERIOD
	Amount of additional time PB to be pressed to register LongPress_10s
*/
static const uint32_t PB_6t10SPRESS_PERIOD = 4000;

/*!\var uint32_t PB_10t20SPRESS_PERIOD
	Amount of additional time PB to be pressed to register LongPress_20s
*/
static const uint32_t PB_10t20SPRESS_PERIOD = 10000;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* PUSHBUTTON FUNCTIONS ------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Poll state of Pushbutton

    \param *state Pointer to Powerui object
    \return PB status (opened or closed)
*/
PBStatus powerui_pb_pollstate(PowerUIState *state);

/*! \brief Pushbutton input Callback Handler

    \param *state Pointer to Powerui object
*/
void powerui_pb_callback(PowerUIState *state);

/*! \brief Pushbutton debounce timer function

    \param xTimer Timer Handle
*/
void powerui_pbenable_clockfn(TimerHandle_t xTimer);

/*! \brief Pushbutton press length timer function

    \param xTimer Timer Handle
*/
void powerui_pbtimer_clockfn(TimerHandle_t xTimer);

#ifdef __cplusplus
}
#endif

#endif /* POWERUI_PB_H_ */
