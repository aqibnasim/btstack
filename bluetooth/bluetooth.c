/*! \file "bluetooth.c"
   \brief Contains bluetooth code and functions
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "bluetooth.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* GLOBAL FUNCTIONS ----------------------------------------------------*/
/*----------------------------------------------------------------------*/

void btInit(BTState *state, AudioState *audio, PeriObject *peri,
		GPIO_TypeDef *shutdown_port, uint16_t shutdown_pin)
{
	//Store pointers
	state->audio = audio;
	state->peri = peri;

	//IO pins
	state->shutdown_port = shutdown_port;
	state->shutdown_pin = shutdown_pin;
	HAL_GPIO_WritePin(state->shutdown_port, state->shutdown_pin, GPIO_PIN_RESET);

	//Enable flags
	state->bt_mode = BT_INITIALISED;
	state->module_enabled = false;
	state->ble_enabled = false;
	state->classic_enabled = false;
	state->audio_enabled = false;

	//Pairing
	state->pairing = false;
	state->paired = false;
}

/*----------------------------------------------------------------------*/

void btModuleControl(BTState *state, bool enable)
{
	if (btIsAirplaneMode(state))
		return;

	if (state->module_enabled == enable)
		return;

	state->module_enabled = enable;

	if (enable)
	{
		//TODO: Enable
		HAL_GPIO_WritePin(state->shutdown_port, state->shutdown_pin, GPIO_PIN_SET);
	}
	else
	{
		//TODO: Disable
		HAL_GPIO_WritePin(state->shutdown_port, state->shutdown_pin, GPIO_PIN_RESET);
	}

	//Wait for boot-up/shutdown to occur
	rtos_Sleep_mS(100);
}

/*----------------------------------------------------------------------*/

void btBLEControl(BTState *state, bool enable)
{
	if (btIsAirplaneMode(state))
		return;

	if (state->ble_enabled == enable)
		return;

	state->ble_enabled = enable;

	if (enable)
	{
		//TODO: Enable BLE
	}
	else
	{
		//TODO: Disable BLE
	}
}

/*----------------------------------------------------------------------*/

void btClassicControl(BTState *state, bool enable)
{
	if (btIsAirplaneMode(state))
		return;

	if (state->classic_enabled == enable)
		return;

	state->classic_enabled = enable;

	if (enable)
	{
		//TODO: Enable BT Classic
	}
	else
	{
		btAudioControl(state, false);

		//TODO: Disable BT Classic
	}
}

/*----------------------------------------------------------------------*/

void btAudioControl(BTState *state, bool enable)
{
	if (state->audio_enabled == enable)
		return;

	state->audio_enabled = enable;

	if (state->audio_enabled)
	{
		state->pairing = true;
		audio_AIF_enable(state->audio, AUDIO_AIF_CH_BT, true);

		//TODO: Enable BT Audio here
		//TODO: Start pairing
	}
	else
	{
		state->pairing = false;
		audio_AIF_enable(state->audio, AUDIO_AIF_CH_BT, false);

		//TODO: Stop pairing
		//TODO: Disable BT Audio
	}

	state->paired = false;
}

/*----------------------------------------------------------------------*/

void btAirplaneMode(BTState *state, bool enable)
{
	if (enable)
	{
		//Enable Airplane mode
		btBLEControl(state, false);
		btClassicControl(state, false);
		btAudioControl(state, false);
		btModuleControl(state, false);

		state->recovery_mode = state->bt_mode;
		state->bt_mode = BT_AIRPLANEMODE;
	}
	else
	{
		//Disable Airplane mode
		state->bt_mode = state->recovery_mode;

		if (state->bt_mode == BT_SLEEPMODE)
		{
			btSleepMode(state);
		}
		else if (state->bt_mode == BT_ACTIVEMODE)
		{
			btActiveMode(state);
		}
	}
}

/*----------------------------------------------------------------------*/

void btAudioPair(BTState *state)
{
	state->pairing = true;

	state->paired = false;

	//TODO: Disconnect from current host
}

/*----------------------------------------------------------------------*/

void btSleepMode(BTState *state)
{
	//Enable BT Module
	//TODO reinstate: Bluetooth module not ready, when unconfigured, has indeterminate
	//behaviour when activated after powerup
	//btModuleControl(&state->bluetooth, true);
	btModuleControl(state, false);

	//Enable BLE
	btBLEControl(state, true);

	//Disable BT Classic
	btClassicControl(state, false);
}

/*----------------------------------------------------------------------*/

void btActiveMode(BTState *state)
{
	//Enable BT Module
	//TODO reinstate: Bluetooth module not ready, when unconfigured, has indeterminate
	//behaviour when activated after powerup
	//btModuleControl(&state->bluetooth, true);
	btModuleControl(state, false);

	//Enable BLE
	btBLEControl(state, true);

	//Enable BT Classic
	btClassicControl(state, true);
}

/*----------------------------------------------------------------------*/

bool btModuleEnabled(BTState *state)
{
	return state->module_enabled;
}

/*----------------------------------------------------------------------*/

bool btBLEEnabled(BTState *state)
{
	return state->ble_enabled;
}

/*----------------------------------------------------------------------*/

bool btClassicEnabled(BTState *state)
{
	return state->classic_enabled;
}

/*----------------------------------------------------------------------*/

bool btAudioEnabled(BTState *state)
{
	return state->audio_enabled;
}

/*----------------------------------------------------------------------*/

bool btIsPairing(BTState *state)
{
	return state->pairing;
}

/*----------------------------------------------------------------------*/

bool btIsPaired(BTState *state)
{
	return state->paired;
}

/*----------------------------------------------------------------------*/

bool btIsAirplaneMode(BTState *state)
{
	if (state->bt_mode == BT_AIRPLANEMODE)
		return true;
	else
		return false;
}

/*----------------------------------------------------------------------*/

void btSendPlayPause(BTState *state)
{
	//TODO: Send Play Pause command
}
