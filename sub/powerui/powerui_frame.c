/*! \file "powerui_frame.c"
   \brief PowerUI FRAME subsystem file
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
#include "powerui.h"
#include "powerui_frame.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FRAME SENSOR FUNCTIONS ----------------------------------------------*/
/*----------------------------------------------------------------------*/
//Poll Battery State of Charge
FrameStatus powerui_frame_pollstate(PowerUIState *state)
{
    return state->frame_status;
}

/*----------------------------------------------------------------------*/
//Check if Status Changed from last poll
bool powerui_frame_statuschanged(PowerUIState *state)
{
    if (state->frame_status != state->frame_status_last)
    {
        state->frame_status_last = state->frame_status;
        return true;
    }
    else
        return false;
}

/*----------------------------------------------------------------------*/
//Update Temple status
void powerui_frame_update(PowerUIState *state)
{
    if (HAL_GPIO_ReadPin(state->hall_port, state->hall_pin))
        state->frame_status = Frame_Opened;
    else
        state->frame_status = Frame_Closed;
}

/*----------------------------------------------------------------------*/

void powerui_frame_callback(PowerUIState *state)
{
	static BaseType_t xHigherPriorityTaskWoken;

	powerui_frame_update(state);

	//Disable interrupt for a bit
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = state->hall_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(state->hall_port, &GPIO_InitStruct);

	xTimerStartFromISR(state->frame_debounce_clock, &xHigherPriorityTaskWoken);
}

/*----------------------------------------------------------------------*/

void powerui_framedebounce_clockfn(TimerHandle_t xTimer)
{
	PowerUIState *state = (PowerUIState*)pvTimerGetTimerID(xTimer);

	//Re-enable interrupt
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = state->hall_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(state->hall_port, &GPIO_InitStruct);
}
