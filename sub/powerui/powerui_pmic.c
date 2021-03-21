/*! \file "powerui_pmic.c"
   \brief PowerUI PMIC subsystem file
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
#include "powerui.h"
#include "powerui_pmic.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* PMIC FUNCTIONS ------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Set PMIC Charge Disable

    \param *state Pointer to PowerUIState object
    \return State of Charge 0-100%
*/
static int8_t powerui_bq_findSOC(PowerUIState *state);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* PMIC FUNCTIONS ------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void powerui_bq_update(PowerUIState *state)
{
    uint8_t tmp = 0;

    //Wait for initialization to complete
    if (!state->initialized)
    	return;

	powerui_bq_ChipEnable(state);

    if (powerui_bq_read_address(state, BQ_REG_STATUS, &tmp)){
        state->status = (tmp & 0xC0) >> 6;
    }

    if (powerui_bq_read_address(state, BQ_REG_FAULT, &tmp)){
        state->fault.bat_ocp = (tmp & 0x10);
        state->fault.bat_uvlo = (tmp & 0x20);
        state->fault.vin_uv = (tmp & 0x40);
        state->fault.vin_ov = (tmp & 0x80);
        if ((state->status == BQ_Fault) && (tmp & 0xF0))
            state->fault.ilim = true;
        else
            state->fault.ilim = false;
    }

    state->soc = powerui_bq_findSOC(state);

	powerui_bq_ChipDisable(state);
}

/*----------------------------------------------------------------------*/

bool powerui_bq_write_address(PowerUIState *state,
                           uint8_t addr, uint8_t data)
{
    uint16_t retry = 0;
    uint8_t *txbuff = (uint8_t*)state->pwr_i2c_trans.write_buff;
    txbuff[0] = data;

    while (1)
    {
        if (!i2cNewTXWait(&state->peri->i2c, &state->pwr_i2c_trans, addr, 1))
        {
            retry++;
            if (retry >= PWR_I2C_RETRIES)
            {
                state->i2c_fail = true;
                return false;
            }
            continue;
        }
        else
            break;
    }

    state->i2c_fail = false;
    return true;
}

/*----------------------------------------------------------------------*/

bool powerui_bq_read_address(PowerUIState *state,
                           uint8_t addr, uint8_t *data)
{
    uint16_t retry = 0;
    uint8_t *rxbuff = (uint8_t*)state->pwr_i2c_trans.read_buff;

    while (1)
    {
        if (!i2cNewRXWait(&state->peri->i2c, &state->pwr_i2c_trans, addr, 1))
        {
            retry++;
            if (retry >= PWR_I2C_RETRIES)
            {
                state->i2c_fail = true;
                return false;
            }
            continue;
        }
        else
        {
            *data = rxbuff[0];
            break;
        }
    }
    state->i2c_fail = false;
    return true;
}

/*----------------------------------------------------------------------*/

void powerui_bq_ChipDisable(PowerUIState *state)
{
	HAL_GPIO_WritePin(state->pmic_port, state->cd_pin, BQ_CD_HIZ);

    rtos_Sleep_mS(CD_SAFETYPERIOD_MS);
}

/*----------------------------------------------------------------------*/

void powerui_bq_ChipEnable(PowerUIState *state)
{
	if (powerui_bq_ChargingCheck(state))
		return;

	HAL_GPIO_WritePin(state->pmic_port, state->cd_pin, BQ_CD_ACTIVE);

    rtos_Sleep_mS(CD_SAFETYPERIOD_MS);
}

/*----------------------------------------------------------------------*/

bool powerui_bq_ChargingCheck(PowerUIState *state)
{
	if (HAL_GPIO_ReadPin(state->pmic_port, state->int_pin) == BQ_INT_CHARGING)
		return true;

	return false;
}

/*----------------------------------------------------------------------*/

int8_t powerui_bq_findSOC(PowerUIState *state)
{
	//Battery voltage as a percentage of vbatreg
	int16_t batV = 0;

	uint8_t tmp;

	if (powerui_bq_write_address(state, BQ_REG_BATMON, 0x80)) {
		//rtos_Sleep_mS(50);
		if (powerui_bq_read_address(state, BQ_REG_BATMON, &tmp)) {
			//Sanity check our input
			if (tmp & 0x80)
				return state->soc;

			batV = (((tmp >> 5) & 0x03) * 10) + 60;
		}
	}

	uint8_t vbmon_th = ((tmp >> 2) & 0x07);

	if (vbmon_th > 0x04)
	{
		batV += ((((tmp >> 2) & 0x03) + 1) << 1);
	}
	else
	{
		batV += ((((tmp >> 2) & 0x03) - 1) << 1);
	}

	batV++;

	//Convert to an SOC percentage
	batV -= BQ_SOC_ZEROVOLTAGE;
	batV = batV * (100 / (100 - BQ_SOC_ZEROVOLTAGE));

	return (int8_t)batV;
}

