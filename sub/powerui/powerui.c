/*! \file "powerui.c"
   \brief Main PowerUI subsystem file

    Contains top level functions for initialization of PowerUI subsystem
    PMIC management
    LED IC management
    Pushbutton input management

    Provides a FREE-RTOS thread that receives commands from other threads
    and executes LED and PMIC routines
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "powerui.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* I2C BUFFERS ---------------------------------------------------------*/
/*----------------------------------------------------------------------*/
uint8_t powerui_read_buff[PWRUI_I2C_RWSIZE];
uint8_t powerui_write_buff[PWRUI_I2C_RWSIZE];

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* PWRUI COMMAND FUNCTIONS ---------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Top Level Command management thread function

    \param *arg Points to the relevant PowerUIState object
*/
static void powerui_task_commands(void *arg);

/*! \brief Reset DSP memory and load program/parameters into CS47L90

    \param *state Pointer to PowerUIState object
    \param *cmd Pointer to Command object
    \return Pass or fail
*/
static bool powerui_init(PowerUIState *state, PowerUICmd *cmd);

/*! \brief Enable LS/LDO output

    \param *state Pointer to PowerUIState object
    \param *cmd Pointer to Command object
    \return Pass or fail
*/
static bool powerui_bq_LDOEnable(PowerUIState *state, PowerUICmd *cmd);

/*! \brief Disable LS/LDO output

    \param *state Pointer to PowerUIState object
    \param *cmd Pointer to Command object
    \return Pass or fail
*/
static bool powerui_bq_LDODisable(PowerUIState *state, PowerUICmd *cmd);

/*! \brief Enable Load Switch output

    \param *state Pointer to PowerUIState object
    \param *cmd Pointer to Command object
    \return Pass or fail
*/
static bool powerui_ls_Enable(PowerUIState *state, PowerUICmd *cmd);

/*! \brief Disable Load Switch output

    \param *state Pointer to PowerUIState object
    \param *cmd Pointer to Command object
    \return Pass or fail
*/
static bool powerui_ls_Disable(PowerUIState *state, PowerUICmd *cmd);

/*! \brief Enable Ship Mode

    \param *state Pointer to PowerUIState object
    \param *cmd Pointer to Command object
    \return Pass or fail
*/
static bool powerui_bq_EnterShipMode(PowerUIState *state, PowerUICmd *cmd);

/*! \brief Off all LED channels

    \param *state Pointer to PowerUIState object
    \param *cmd Pointer to Command object
    \return Pass or fail
*/
static bool powerui_ncp_LEDOff(PowerUIState *state, PowerUICmd *cmd);

/*! \brief Set LED Pairing mode

    \param *state Pointer to PowerUIState object
    \param *cmd Pointer to Command object
    \return Pass or fail
*/
static bool powerui_ncp_PairStart(PowerUIState *state, PowerUICmd *cmd);

/*! \brief Set LED Charging mode

    \param *state Pointer to PowerUIState object
    \param *cmd Pointer to Command object
    \return Pass or fail
*/
static bool powerui_ncp_ChargeStart(PowerUIState *state, PowerUICmd *cmd);

/*! \brief Set LED Low battery mode

    \param *state Pointer to PowerUIState object
    \param *cmd Pointer to Command object
    \return Pass or fail
*/
static bool powerui_ncp_LowbatStart(PowerUIState *state, PowerUICmd *cmd);

/*! \brief Load Defaults into NVS
 	 Only used if PowerUI Config in NVS empty

    \param *state Pointer to PowerUIState object
    \return Pass or fail
*/
static bool poweruiLoadDefaultsToNVS(PowerUIState *state);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* SYSTEM CONTROL ------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void poweruiInit(PowerUIState *state, int priority, PeriObject *peri,
		GPIO_TypeDef *pmic_port,
		uint16_t cd_pin, uint16_t lscntrl_pin, uint16_t int_pin,
		uint16_t ldo1v2en_pin, uint16_t lsen_pin,
		GPIO_TypeDef *hall_port, uint16_t hall_pin,
		GPIO_TypeDef *pb_port, uint16_t pb_pin)
{
    //Set I2C
    state->peri = peri;
    i2cInitTransaction(&state->pwr_i2c_trans, BQ_I2C_ADDR, I2C_MEMADD_SIZE_8BIT,
                       (void*)powerui_read_buff, (void*)powerui_write_buff);
    i2cInitTransaction(&state->led_i2c_trans, NCP_I2C_ADDR, I2C_MEMADD_SIZE_8BIT,
                       (void*)powerui_read_buff, (void*)powerui_write_buff);

    //PMIC/LS Parameters
    state->pmic_port = pmic_port;
    state->cd_pin = cd_pin;
	state->lscntrl_pin = lscntrl_pin;
	state->int_pin = int_pin;
	state->ldo1v2en_pin = ldo1v2en_pin;
	state->lsen_pin = lsen_pin;

    //Enable I2C (toggle CD pin)
    //CHGEN: I2C only works when Vin is applied
    powerui_bq_ChipDisable(state);

    //Initialize SOC before first read
    state->soc = 100;

    //LED
    state->led_active_animation = LED_Anim_None;
    state->animations = LED_ANIMATIONS;
    rtos_createClock(&state->led_timer, "LED Animation timer", priority, 1,
            		true, powerui_ledtimer_clockfn, (void *) state);

    //Frame sensor
    state->hall_port = hall_port;
    state->hall_pin = hall_pin;
    powerui_frame_update(state);
    state->frame_status_last = state->frame_status;
    rtos_createClock(&state->frame_debounce_clock, "Frame sensor debounce timer", priority, FRM_DEBOUNCE_PERIODMS,
        		true, powerui_framedebounce_clockfn, (void *) state);

    //Pushbutton
    state->pb_port = pb_port;
    state->pb_pin = pb_pin;
    state->pb_status = PB_NoClick;
    rtos_createClock(&state->pbPressEnable_clock, "Pushbutton debounce timer", priority, PB_PRESSENABLE_PERIOD,
            		true, powerui_pbenable_clockfn, (void *) state);
    rtos_createClock(&state->pbPressTimer_clock, "Pushbutton press length timer", priority, PB_3SPRESS_PERIOD,
            		true, powerui_pbtimer_clockfn, (void *) state);

    rtos_createMailbox(&state->mail_cmd, sizeof(PowerUICmd), POWER_MB_SIZE);

    rtos_createThread(&state->task_cmd, priority, POWER_STACK_SIZE,
    		powerui_task_commands, "PowerUI Command Task", (void *) state);
}

/*----------------------------------------------------------------------*/
//Post a command to Power Mailbox
void powerui_post_cmd(PowerUIState *state, PowerUICmdType cmd, SemaphoreHandle_t *sem)
{
    PowerUICmd power_cmd;
    power_cmd.cmd = cmd;
    power_cmd.items.sem_cmd = sem;
    power_cmd.items.success = false;

    xQueueSend(state->mail_cmd, &power_cmd, 0);
}


/*----------------------------------------------------------------------*/
//Post a command to Power Mailbox and wait
bool powerui_post_cmdwait(PowerUIState *state, PowerUICmdType cmd, SemaphoreHandle_t *sem, uint32_t waitcnt)
{
    powerui_post_cmd(state, cmd, sem);
    bool result = xSemaphoreTake(*sem, waitcnt);

    return result;
}

/*----------------------------------------------------------------------*/
//Poll Battery State of Charge
int8_t poweruiChkSOC(PowerUIState *state)
{
    return state->soc;
}

/*----------------------------------------------------------------------*/

bool poweruiLoadDefaultsToNVS(PowerUIState *state)
{
    MemElement temp_elem;
	uint16_t elemindex;

#ifndef PWRUI_LOAD_FROM_RAM
	//Retrieve and load power configuration
	if (memReadElementParam(&state->peri->mem, &state->peri->mem.chunks[PWRUI_MEMCHUNK_PWRUI_IND],
			&temp_elem, PWRUI_CFG_MEMELEM_INDEX) != MEM_OP_SUCCESS)
	{
		//Erase and initialise chunk
		if (memInitChunk(&state->peri->mem, &state->peri->mem.chunks[PWRUI_MEMCHUNK_PWRUI_IND]) != MEM_OP_SUCCESS)
			return false;

		if (memAddElement(&state->peri->mem, &state->peri->mem.chunks[PWRUI_MEMCHUNK_PWRUI_IND],
						  BQ_CFGADDR_NUM * sizeof(BQConfigReg), &temp_elem, &elemindex)
				!= MEM_OP_SUCCESS)
			return false;

		if (memWriteToElement(&state->peri->mem, &state->peri->mem.chunks[PWRUI_MEMCHUNK_PWRUI_IND], &temp_elem,
							  0, BQ_CONFIG_DEFAULTS, BQ_CFGADDR_NUM * sizeof(BQConfigReg))
				!= MEM_OP_SUCCESS)
			return false;
	}

	//Retrieve and load LED configuration
	if (memReadElementParam(&state->peri->mem, &state->peri->mem.chunks[PWRUI_MEMCHUNK_LED_IND],
			&temp_elem, PWRUI_LEDANIM_MEMELEM_INDEX) != MEM_OP_SUCCESS)
	{
		//Erase and initialise chunk
		if (memInitChunk(&state->peri->mem, &state->peri->mem.chunks[PWRUI_MEMCHUNK_LED_IND]) != MEM_OP_SUCCESS)
			return false;

		if (memAddElement(&state->peri->mem, &state->peri->mem.chunks[PWRUI_MEMCHUNK_LED_IND],
				PWRUI_LED_NUMANIM * sizeof(LEDAnim), &temp_elem, &elemindex)
				!= MEM_OP_SUCCESS)
			return false;

		if (memWriteToElement(&state->peri->mem, &state->peri->mem.chunks[PWRUI_MEMCHUNK_LED_IND], &temp_elem,
							  0, LED_ANIMATIONS, PWRUI_LED_NUMANIM * sizeof(LEDAnim))
				!= MEM_OP_SUCCESS)
			return false;
	}

#endif
    return true;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* COMMAND FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------*/

void powerui_task_commands(void *arg)
{
    PowerUIState *state = (PowerUIState *)arg;
    PowerUICmd cmd;

    //Read PMIC status before starting loop
	powerui_bq_update(state);

    while (1)
    {
        //Wait for new command to come in
    	if (!xQueueReceive(state->mail_cmd, &cmd, PWR_STATUPD_PERIODMS))
    	{
    		powerui_bq_update(state);
    		continue;
    	}

        switch (cmd.cmd)
        {
        case PWRUI_CMD_INIT:
            if (!powerui_init(state, &cmd))
                rtos_cmd_fail(&cmd.items);

            break;
        case PWR_CMD_LSLDO_EN:
            if (!powerui_bq_LDOEnable(state, &cmd))
                rtos_cmd_fail(&cmd.items);

            break;
        case PWR_CMD_LSLDO_DIS:
            if (!powerui_bq_LDODisable(state, &cmd))
                rtos_cmd_fail(&cmd.items);

            break;
        case PWR_CMD_LS_EN:
        	if (!powerui_ls_Enable(state, &cmd))
				rtos_cmd_fail(&cmd.items);
            break;
        case PWR_CMD_LS_DIS:
        	if (!powerui_ls_Disable(state, &cmd))
				rtos_cmd_fail(&cmd.items);
            break;
        case PWR_CMD_ENTERSHIP:
        	if (!powerui_bq_EnterShipMode(state, &cmd))
				rtos_cmd_fail(&cmd.items);
        	break;
        case LED_CMD_OFF:
            if (!powerui_ncp_LEDOff(state, &cmd))
                rtos_cmd_fail(&cmd.items);
            break;
        case LED_CMD_PAIR_START:
            if (!powerui_ncp_PairStart(state, &cmd))
                rtos_cmd_fail(&cmd.items);
            break;
        case LED_CMD_CHARGE_START:
			if (!powerui_ncp_ChargeStart(state, &cmd))
				rtos_cmd_fail(&cmd.items);
			break;
		case LED_CMD_LOWBAT_START:
			if (!powerui_ncp_LowbatStart(state, &cmd))
				rtos_cmd_fail(&cmd.items);
			break;
        }
    }
}

/*----------------------------------------------------------------------*/
//Initialization routine
bool powerui_init(PowerUIState *state, PowerUICmd *cmd)
{
	MemElement temp_cfg_elem;
	MemElement temp_anim_elem;

	bool result = true;

	powerui_bq_ChipEnable(state);

#ifndef PWRUI_LOAD_FROM_RAM
	//Load defaults into Memory if none available
	poweruiLoadDefaultsToNVS(state);
	memReadElementParam(&state->peri->mem, &state->peri->mem.chunks[PWRUI_MEMCHUNK_PWRUI_IND],
				&temp_cfg_elem, PWRUI_CFG_MEMELEM_INDEX);
	memReadElementParam(&state->peri->mem, &state->peri->mem.chunks[PWRUI_MEMCHUNK_LED_IND],
					&temp_anim_elem, PWRUI_LEDANIM_MEMELEM_INDEX);
#endif

	//Setup PMIC
    BQConfigReg temp_config;
    for (size_t i=0; i < BQ_CFGADDR_NUM; i++)
    {
#ifndef PWRUI_LOAD_FROM_RAM
        if (memReadFromElement(&state->peri->mem, &state->peri->mem.chunks[PWRUI_MEMCHUNK_PWRUI_IND],
                               &temp_cfg_elem, i * sizeof(BQConfigReg),
                               &temp_config, sizeof(BQConfigReg)) != MEM_OP_SUCCESS)
        {
        	result = false;
        	goto END;
        }
#else
        temp_config = BQ_CONFIG_DEFAULTS[i];
#endif
        //Store LSLDO value
        if (temp_config.addr == BQ_REG_LSLDO)
            state->LDO_val = temp_config.data;

        if (!powerui_bq_write_address(state, temp_config.addr, temp_config.data))
        {
        	result = false;
        	goto END;
        }
    }

    //Setup LED
    LEDAnim temp_anim;
    for (uint8_t i = 0; i < PWRUI_LED_NUMANIM; i++)
    {
#ifndef PWRUI_LOAD_FROM_RAM
		if (memReadFromElement(&state->peri->mem, &state->peri->mem.chunks[PWRUI_MEMCHUNK_LED_IND],
							   &temp_anim_elem, i * sizeof(LEDAnim),
							   &temp_anim, sizeof(LEDAnim)) != MEM_OP_SUCCESS)
		{
			result = false;
			goto END;
		}
#else
		temp_anim = LED_ANIMATIONS[i];
#endif

		state->animations[i] = temp_anim;
    }

    //Allow other commands to execute
    state->initialized = true;

END:
	powerui_bq_ChipDisable(state);

    //Success
    rtos_cmd_success(&cmd->items);

    return result;
}

/*----------------------------------------------------------------------*/
//Enable LDO rail
bool powerui_bq_LDOEnable(PowerUIState *state, PowerUICmd *cmd)
{
    if (!state->initialized) {
        return false;
    }

	powerui_bq_ChipEnable(state);

    uint8_t data = 0;

#ifndef PWRUI_LCNTRL
    powerui_bq_read_address(state, BQ_REG_LSLDO, &data);

    data |= 0x80;

    powerui_bq_write_address(state, BQ_REG_LSLDO, data);
#else
    data = state->LDO_val | 0x80;
    powerui_bq_write_address(state, BQ_REG_LSLDO, data);

    HAL_GPIO_WritePin(state->pmic_port, state->lscntrl_pin, BQ_LSCNTRL_EN);

#endif

    rtos_Sleep_mS(100);

    HAL_GPIO_WritePin(state->pmic_port, state->ldo1v2en_pin, BQ_1V2EN_EN);

	powerui_bq_ChipDisable(state);

	//Success
    rtos_cmd_success(&cmd->items);

    return true;
}

/*----------------------------------------------------------------------*/
//Disable LDO rail
bool powerui_bq_LDODisable(PowerUIState *state, PowerUICmd *cmd)
{
    if (!state->initialized) {
        return false;
    }

	powerui_bq_ChipEnable(state);

    uint8_t data = 0;

#ifndef PWRUI_LCNTRL
	powerui_bq_read_address(state, BQ_REG_LSLDO, &data);

	data &= ~(0x80);

	powerui_bq_write_address(state, BQ_REG_LSLDO, data);
#else
    HAL_GPIO_WritePin(state->pmic_port, state->lscntrl_pin, BQ_LSCNTRL_DIS);

    powerui_bq_write_address(state, BQ_REG_LSLDO, 0);
#endif

    rtos_Sleep_mS(100);

	HAL_GPIO_WritePin(state->pmic_port, state->ldo1v2en_pin, BQ_1V2EN_DIS);

	powerui_bq_ChipDisable(state);

    //Success
    rtos_cmd_success(&cmd->items);

    return true;
}

/*----------------------------------------------------------------------*/
//Enable Load switch output
bool powerui_ls_Enable(PowerUIState *state, PowerUICmd *cmd)
{
    if (!state->initialized) {
        return false;
    }

    HAL_GPIO_WritePin(state->pmic_port, state->lsen_pin, VBAT_LS_EN);

    //Success
    rtos_cmd_success(&cmd->items);

    return true;
}

/*----------------------------------------------------------------------*/
//Disable Load switch output
bool powerui_ls_Disable(PowerUIState *state, PowerUICmd *cmd)
{
	if (!state->initialized) {
		return false;
	}

	HAL_GPIO_WritePin(state->pmic_port, state->lsen_pin, VBAT_LS_DIS);

	//Success
	rtos_cmd_success(&cmd->items);

	return true;
}

/*----------------------------------------------------------------------*/
//Enter Ship Mode
bool powerui_bq_EnterShipMode(PowerUIState *state, PowerUICmd *cmd)
{
	if (!state->initialized) {
		return false;
	}

	bool result = true;

	powerui_bq_ChipEnable(state);

	uint8_t tmp;
	if (powerui_bq_read_address(state, BQ_REG_STATUS, &tmp))
	{
		if (!powerui_bq_write_address(state, BQ_REG_STATUS, (tmp | 0x20)))
		{
			result = false;
			goto END;
		}
	}

END:
	powerui_bq_ChipDisable(state);

	//Success
	rtos_cmd_success(&cmd->items);

	return result;
}

/*----------------------------------------------------------------------*/
//Turn LED off
bool powerui_ncp_LEDOff(PowerUIState *state, PowerUICmd *cmd)
{
	//Stop animation
	xTimerStop(state->led_timer, 0);
	state->led_active_animation = LED_Anim_None;

	if (!powerui_ncp_currentstep(state, 0))
		return false;

    if (!powerui_ncp_shutdown(state))
        return false;

    //Success
    rtos_cmd_success(&cmd->items);
    return true;
}

/*----------------------------------------------------------------------*/

bool powerui_ncp_PairStart(PowerUIState *state, PowerUICmd *cmd)
{
	state->led_active_animation = LED_Anim_Pairing;

	if (!powerui_ncp_startanimation(state, &state->animations[LED_ANIM_PAIR_IND]))
		return false;

    //Success
    rtos_cmd_success(&cmd->items);
    return true;
}

/*----------------------------------------------------------------------*/

bool powerui_ncp_ChargeStart(PowerUIState *state, PowerUICmd *cmd)
{
	state->led_active_animation = LED_Anim_Charging;

	if (!powerui_ncp_startanimation(state, &state->animations[LED_ANIM_CHARGE_IND]))
		return false;

    //Success
    rtos_cmd_success(&cmd->items);
    return true;
}

/*----------------------------------------------------------------------*/

bool powerui_ncp_LowbatStart(PowerUIState *state, PowerUICmd *cmd)
{
	state->led_active_animation = LED_Anim_Lowbattery;

	if (!powerui_ncp_startanimation(state, &state->animations[LED_ANIM_LOWBAT_IND]))
		return false;

    //Success
    rtos_cmd_success(&cmd->items);
    return true;
}






