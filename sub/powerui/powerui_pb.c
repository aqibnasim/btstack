/*! \file "powerui_pb.c"
   \brief PowerUI PB subsystem file
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
#include "powerui.h"
#include "powerui_pb.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* PUSHBUTTON FUNCTIONS ------------------------------------------------*/
/*----------------------------------------------------------------------*/

PBStatus powerui_pb_pollstate(PowerUIState *state)
{
	return state->pb_status;
}

/*----------------------------------------------------------------------*/

void powerui_pb_callback(PowerUIState *state)
{
	static BaseType_t xHigherPriorityTaskWoken;

	if (HAL_GPIO_ReadPin(state->pb_port, state->pb_pin))
	{
		//PB Released
		xTimerStopFromISR(state->pbPressTimer_clock, &xHigherPriorityTaskWoken);
		if (powerui_pb_pollstate(state) == PB_NoClick)
		{
			state->pb_status = PB_SingleClick;
		}
	}
	else
	{
		//PB Pressed
		state->pb_status = PB_NoClick;
		xTimerChangePeriodFromISR(state->pbPressTimer_clock, PB_3SPRESS_PERIOD, &xHigherPriorityTaskWoken);
		xTimerStartFromISR(state->pbPressTimer_clock, &xHigherPriorityTaskWoken);
	}

	//Disable interrupt for a bit
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = state->pb_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(state->pb_port, &GPIO_InitStruct);

	xTimerStartFromISR(state->pbPressEnable_clock, &xHigherPriorityTaskWoken);
}

/*----------------------------------------------------------------------*/

void powerui_pbenable_clockfn(TimerHandle_t xTimer)
{
	PowerUIState *state = (PowerUIState*)pvTimerGetTimerID(xTimer);

	//Re-enable interrupt
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = state->pb_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(state->pb_port, &GPIO_InitStruct);
}

/*----------------------------------------------------------------------*/

void powerui_pbtimer_clockfn(TimerHandle_t xTimer)
{
	PowerUIState *state = (PowerUIState*)pvTimerGetTimerID(xTimer);

	if (HAL_GPIO_ReadPin(state->pb_port, state->pb_pin) == GPIO_PIN_SET)
		return;

	if (powerui_pb_pollstate(state) < PB_LongPress_3s)
	{
		state->pb_status = PB_LongPress_3s;
		xTimerChangePeriod(state->pbPressTimer_clock, PB_3t6SPRESS_PERIOD, 0);
		xTimerStart(state->pbPressTimer_clock, 0);
	}
	else if (powerui_pb_pollstate(state) < PB_LongPress_6s)
	{
		state->pb_status = PB_LongPress_6s;
		xTimerChangePeriod(state->pbPressTimer_clock, PB_6t10SPRESS_PERIOD, 0);
		xTimerStart(state->pbPressTimer_clock, 0);
	}
	else if (powerui_pb_pollstate(state) < PB_LongPress_10s)
	{
		state->pb_status = PB_LongPress_10s;
		xTimerChangePeriod(state->pbPressTimer_clock, PB_10t20SPRESS_PERIOD, 0);
		xTimerStart(state->pbPressTimer_clock, 0);
	}
	else if (powerui_pb_pollstate(state) < PB_LongPress_20s)
	{
		state->pb_status = PB_LongPress_20s;
	}
}
