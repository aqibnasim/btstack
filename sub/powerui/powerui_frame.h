/*! \file "powerui_frame.h"
   \brief PowerUI FRAME subsystem file
*/

#ifndef POWERUI_FRAME_H_
#define POWERUI_FRAME_H_

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
/* FRAME SENSOR CONSTANTS ----------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\var uint32_t FRM_DEBOUNCE_PERIODMS
	Amount of time to debounce Frame sensor input
*/
static const uint32_t FRM_DEBOUNCE_PERIODMS = 50;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FRAME SENSOR FUNCTIONS ----------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Update Frame state

    \param *state Pointer to Powerui object
*/
void powerui_frame_update(PowerUIState *state);

/*! \brief Frame debounce timer function

    \param xTimer Timer Handle
*/
void powerui_framedebounce_clockfn(TimerHandle_t xTimer);

/*! \brief Poll state of Frame

    \param *state Pointer to Powerui object
    \return Frame status (opened or closed)
*/
FrameStatus powerui_frame_pollstate(PowerUIState *state);

/*! \brief Check if frame status changed from last time executing function

    \param *state Pointer to Powerui object
    \return Status changed
*/
bool powerui_frame_statuschanged(PowerUIState *state);

/*! \brief Frame input Callback Handler

    \param *state Pointer to Powerui object
*/
void powerui_frame_callback(PowerUIState *state);

#ifdef __cplusplus
}
#endif

#endif /* POWERUI_FRAME_H_ */
