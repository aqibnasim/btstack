/*! \file "powerui_led.c"
   \brief PowerUI LED subsystem file
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
#include "powerui.h"
#include "powerui_led.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* LED IC FUNCTIONS ----------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Write to LED IC

    \param *state Pointer to PowerUIState object
    \param addr Address to write to
    \param data Data to write
    \return Pass or fail
*/
static bool powerui_ncp_write_address(PowerUIState *state, uint8_t addr, uint8_t data);

/*! \brief Set LED IC Channel PWM level

    \param *state Pointer to PowerUIState object
    \param ledch LED Channel
    \param pwm PWM Level
    \return Pass or fail
*/
static bool powerui_ncp_ledpwm(PowerUIState *state, uint8_t ledch, uint8_t pwm);

/*! \brief Set LED IC Sequence

    \param *state Pointer to PowerUIState object
    \param *anim Pointer to animation being played
    \return Pass or fail
*/
static bool powerui_ncp_setseq(PowerUIState *state, LEDAnimState dir, LEDAnim *anim);

/*! \brief Set the animation timer period based on animation state, and execute

    \param *state Pointer to PowerUIState object
    \param *anim Pointer to animation being played
    \return Pass or fail
*/
static bool powerui_ncp_setanimationtimer(PowerUIState *state, LEDAnim *anim);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* LED IC FUNCTIONS ----------------------------------------------------*/
/*----------------------------------------------------------------------*/

bool powerui_ncp_write_address(PowerUIState *state,
                           uint8_t addr, uint8_t data)
{
    uint8_t *txbuff = (uint8_t*)state->led_i2c_trans.write_buff;
    txbuff[0] = (addr << 5) | data;

    if (!i2cNewTXWait(&state->peri->i2c, &state->led_i2c_trans, I2C_NOADDR, 1))
    {
        state->i2c_fail = true;
        return false;
    }

    state->i2c_fail = false;
    return true;
}

/*----------------------------------------------------------------------*/

bool powerui_ncp_shutdown(PowerUIState *state)
{
    return powerui_ncp_write_address(state, NCP_REG_CSHUTDOWN, 0);
}

/*----------------------------------------------------------------------*/

bool powerui_ncp_currentstep(PowerUIState *state, uint8_t step)
{
    uint8_t data = step;

    return powerui_ncp_write_address(state, NCP_REG_CURRSTEP, data);
}

/*----------------------------------------------------------------------*/

bool powerui_ncp_ledpwm(PowerUIState *state, uint8_t ledch, uint8_t pwm)
{
    uint8_t data = 0;

    if (pwm > NCP_MAX_PWM)
        pwm = NCP_MAX_PWM;

    if (ledch >= NCP_MAX_LEDCH)
        ledch = NCP_MAX_LEDCH;

    data = pwm;

    return powerui_ncp_write_address(state, (NCP_REG_LEDPWMBASE + (NCP_REG_LEDREGJUMP * ledch)),
                                     data);
}

/*----------------------------------------------------------------------*/

bool powerui_ncp_setseq(PowerUIState *state, LEDAnimState dir, LEDAnim *anim)
{
	uint8_t iend_reg;
	uint8_t target;

	if (dir == LED_Animstate_downward)
	{
		iend_reg = NCP_REG_IENDDWN;
		target = anim->bot_iend;
	}
	else
	{
		iend_reg = NCP_REG_IENDUP;
		target = anim->top_iend;
	}

	if (!powerui_ncp_write_address(state, iend_reg, target))
		return false;

	return powerui_ncp_write_address(state, NCP_REG_STEPTIME, anim->step_time);
}

/*----------------------------------------------------------------------*/

void powerui_ledtimer_clockfn(TimerHandle_t xTimer)
{
	PowerUIState *state = (PowerUIState*)pvTimerGetTimerID(xTimer);

	if (state->anim_state == LED_Animstate_downward)
		state->anim_state = LED_Animstate_upward;
	else
		state->anim_state = LED_Animstate_downward;

	uint16_t retries = 0;
	while (retries < PWRUI_LEDANIM_RETRIES)
	{
		if (powerui_ncp_setseq(state, state->anim_state, state->anim_active))
		{
			powerui_ncp_setanimationtimer(state, state->anim_active);
			break;
		}
		else
		{
			retries++;
		}
	}

	//If we really can't write a sequence, shutoff LED
	if (retries == PWRUI_LEDANIM_RETRIES)
	{
		powerui_ncp_shutdown(state);
	}
}

/*----------------------------------------------------------------------*/

bool powerui_ncp_setanimationtimer(PowerUIState *state, LEDAnim *anim)
{
	uint32_t hold_period = 0;
	if (state->anim_state == LED_Animstate_downward)
		hold_period = anim->bothold_period;
	else
		hold_period = anim->tophold_period;

	uint32_t timer_period = hold_period + ((anim->step_time << 3) * (anim->top_iend - anim->bot_iend)) + 1;

	xTimerChangePeriod(state->led_timer, timer_period, 0);
	xTimerStart(state->led_timer, 0);

	return true;
}

/*----------------------------------------------------------------------*/

bool powerui_ncp_startanimation(PowerUIState *state, LEDAnim *anim)
{
	state->anim_active = anim;

	state->anim_state = LED_Animstate_upward;

	if (!powerui_ncp_shutdown(state))
		return false;

	if (!powerui_ncp_currentstep(state, state->anim_active->bot_iend))
		return false;

	rtos_Sleep_mS(10);

	if (!powerui_ncp_ledpwm(state, state->anim_active->channel, state->anim_active->pwm))
		return false;



	if (!powerui_ncp_setseq(state, state->anim_state, state->anim_active))
		return false;

	//Setup clock
	if (!powerui_ncp_setanimationtimer(state, state->anim_active))
		return false;

	return true;
}
