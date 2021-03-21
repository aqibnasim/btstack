/*! \file "statemachine.c"
   \brief Contains state machine code and functions
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "statemachine.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Initialize all peripherals

    \param *state Pointer to mainState object
*/
static void smInitPeripherals(mainState *state);

/*! \brief Initialize all sub systems

    \param *state Pointer to mainState object
*/
static void smInitSubsystems(mainState *state);

/*! \brief Execute a Soft Reset of system

    \param *state Pointer to mainState object
*/
static void smStateSoftReset(mainState *state);

/*! \brief Enter Low Power mode

    \param *state Pointer to mainState object
*/
static void smTransitionEnterLowPower(mainState *state);

/*! \brief Exit Low Power mode

    \param *state Pointer to mainState object
*/
static void smTransitionExitLowPower(mainState *state);

/*! \brief Enter Sleep mode

    \param *state Pointer to mainState object
*/
static void smTransitionEnterSleep(mainState *state);

/*! \brief Enter Active mode

    \param *state Pointer to mainState object
*/
static void smTransitionEnterActive(mainState *state);

/*! \brief Manage Pushbutton inputs

    \param *state Pointer to mainState object
*/
static void smPushbuttonManage(mainState *state);

/*! \brief Handle user notifications

    \param *state Pointer to mainState object
*/
static void smUserNotifications(mainState *state);

/*! \brief Handle LED notifications

    \param *state Pointer to mainState object
*/
static void smLEDNotifications(mainState *state);

/*! \brief Handle Audio notifications

    \param *state Pointer to mainState object
*/
static void smAudioNotifications(mainState *state);

/*! \brief Enable user notifications

    \param *state Pointer to mainState object
*/
static void smEnableUserNotifications(mainState *state);

/*! \brief Disable user notifications

    \param *state Pointer to mainState object
*/
static void smDisableUserNotifications(mainState *state);

/*! \brief Control load switch

    \param *state Pointer to mainState object
    \param enable Enable or Disable output
*/
static void smLoadSwitchControl(mainState *state, bool enable);

/*! \brief Set uC to use Sleep as Low Power mode

    \param *state Pointer to mainState object
*/
static void smSetLowPowerSleep(mainState *state);

/*! \brief Set uC to use Stop as Low Power mode

    \param *state Pointer to mainState object
*/
static void smSetLowPowerStop(mainState *state);

/*! \brief Enter uC SLEEP mode

    \param *state Pointer to mainState object
*/
static void smEnterSLEEPMode(mainState *state);

/*! \brief Exit uC SLEEP mode

    \param *state Pointer to mainState object
*/
static void smExitSLEEPMode(mainState *state);

/*! \brief Enter uC STOP mode

    \param *state Pointer to mainState object
*/
static void smEnterSTOPMode(mainState *state);

/*! \brief Exit uC STOP mode

    \param *state Pointer to mainState object
*/
static void smExitSTOPMode(mainState *state);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* MAIN THREAD ---------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void stateMachineTask(void *arg)
{
    mainState *state = (mainState *)arg;

    state->current_state = STATE_STARTUP;
    state->pb_laststate = PB_NoClick;

    //Initialize peripherals and subsystems
    smInitPeripherals(state);
    smInitSubsystems(state);

    rtos_createSemaphore(&state->semhnd_cmd, 1, 0);

    //Loop power initialization till we're ready
    //while (!powerui_post_cmdwait(&state->power, PWRUI_CMD_INIT, &state->semhnd_cmd, 500));

    //Initialize based on whether frame opened or closed
    /*if (powerui_frame_pollstate(&state->power) == Frame_Opened)
	{
		smTransitionEnterActive(state);
	}
	else
	{
		smTransitionEnterSleep(state);
	}*/

    smTransitionEnterActive(state);

    //TODO remove
    //Set Pairing animation to start on bootup. This is a stop gap UI indicator for testing. Get rid of it
    //once bluetooth is ready
    state->bluetooth.pairing = true;

    //Start loop
    while (1) {

   		rtos_Sleep_mS(SM_LOOP_PERIODMS);

        //Check battery state
        /*if (!powerui_bq_ChargingCheck(&state->power) && poweruiChkSOC(&state->power) <= SM_LOWPOWER_PERCENT)
        {
        	smTransitionEnterLowPower(state);
        	continue;
        }
        else
        {
        	smTransitionExitLowPower(state);
        }*/

        //Deal with system control commands
        if (state->soft_reset)
		{
			state->soft_reset = false;
			smStateSoftReset(state);
		}
		else if (state->sleep_cmd)
		{
			state->sleep_cmd = false;
			smTransitionEnterSleep(state);
		}
		else if (state->wake_cmd)
		{
			state->wake_cmd = false;
			smTransitionEnterActive(state);
		}

        //Deal with Frame open/close
        /*if (powerui_frame_statuschanged(&state->power))
        {
            if (powerui_frame_pollstate(&state->power) == Frame_Opened)
            {
            	smTransitionEnterActive(state);
            }
            else
            {
            	smTransitionEnterSleep(state);
            }
        }*/

        //Process Pushbutton inputs
        //smPushbuttonManage(state);

        //User notifications
        //smUserNotifications(state);
    }
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INITIALISATIONS -----------------------------------------------------*/
/*----------------------------------------------------------------------*/

void smInitPeripherals(mainState *state)
{
    i2cInit(&state->peripherals.i2c, &hi2c1);
#ifndef AUD_EXTSPI
    spiInit(&state->peripherals.spi, &hspi2, 1);
#endif
    /*memInit(&state->peripherals.mem, &hspi1, 0,
    		MEM_CS1_GPIO_Port, MEM_CS1_Pin,
            MEMMAP_CHUNK, MEMCHUNK_TOTAL, MEM_SECTOR_SIZE, MEM_TOTAL_SIZE);
    memInit(&state->peripherals.mem_aud, &hspi1, 0,
        		MEM_CS2_GPIO_Port, MEM_CS2_Pin,
				AUDMAP_CHUNK, AUDCHUNK_TOTAL, MEM_SECTOR_SIZE, MEM_TOTAL_SIZE);*/
    //i2sInit(&state->peripherals.i2s, &hi2s3, 0);
}

/*----------------------------------------------------------------------*/

void smInitSubsystems(mainState *state)
{
   /* audioInit(&state->audio, &state->power, &state->peripherals,
		   DSP_CS_GPIO_Port, DSP_CS_Pin);

    poweruiInit(&state->power, PWRUI_RTOS_PRIORITY, &state->peripherals,
    		C_CD_GPIO_Port, C_CD_Pin, LSCNTRL_Pin, PM_INT_Pin, EN_1V2_Pin, LS_EN_Pin,
			FRM_GPIO_Port, FRM_Pin, PBIN_GPIO_Port, PBIN_Pin);*/

    btInit(&state->bluetooth, &state->audio,  &state->peripherals,
    		BT_NSHUTDOWN_GPIO_Port, BT_NSHUTDOWN_Pin);
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* STATE COMMANDS ------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void smQueueSoftReset(mainState *state)
{
    state->soft_reset = true;
}

/*----------------------------------------------------------------------*/

void smStateSoftReset(mainState *state)
{
	smTransitionEnterSleep(state);
	smTransitionEnterActive(state);
}

/*----------------------------------------------------------------------*/

void smQueueSleep(mainState *state)
{
    state->sleep_cmd = true;
}

/*----------------------------------------------------------------------*/

void smQueueWake(mainState *state)
{
    state->wake_cmd = true;
}


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* STATE TRANSITIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------*/

void smTransitionEnterLowPower(mainState *state)
{
	if (state->current_state == STATE_LOWPOWER)
		return;

	if (state->current_state == STATE_ACTIVE)
	{
		smTransitionEnterSleep(state);
		state->recovery_state = STATE_ACTIVE;
	}
	else
		state->recovery_state = STATE_SLEEP;

	state->current_state = STATE_ENTERLOWPOWER;

	//Disable BLE
	btBLEControl(&state->bluetooth, false);

	//BT Module Shutdown
	btModuleControl(&state->bluetooth, false);

	smLoadSwitchControl(state, false);

	state->current_state = STATE_LOWPOWER;
}

/*----------------------------------------------------------------------*/

void smTransitionExitLowPower(mainState *state)
{
	if (state->current_state != STATE_LOWPOWER)
		return;

	if (state->recovery_state == STATE_ACTIVE)
	{
		smTransitionEnterActive(state);
	}
	else
	{
		smTransitionEnterSleep(state);
	}
}

/*----------------------------------------------------------------------*/

void smTransitionEnterSleep(mainState *state)
{
	if (state->current_state == STATE_SLEEP)
		return;

	state->current_state = STATE_ENTERSLEEP;

	//Disable Audio subsystem
	//audio_cs_slpenter(&state->audio);

	//Enable LS output
	//smLoadSwitchControl(state, true);

	//Bluetooth sleep mode
	btSleepMode(&state->bluetooth);

	//Disable User notifications
	//smDisableUserNotifications(state);

	//Set microcontroller idle power state to STOP
	//smSetLowPowerStop(state);

	state->current_state = STATE_SLEEP;
}

/*----------------------------------------------------------------------*/

void smTransitionEnterActive(mainState *state)
{
	if (state->current_state == STATE_ACTIVE)
		return;

	state->current_state = STATE_ENTERACTIVE;

	//Set microcontroller idle power state to SLEEP
	smSetLowPowerSleep(state);

	//Enable LS output
	//smLoadSwitchControl(state, true);

	//Bluetooth active mode
	btActiveMode(&state->bluetooth);

	//Enable User notifications
	//smEnableUserNotifications(state);

	//Enable Audio subsystem
	//audio_cs_init(&state->audio);

	//Play Startup audio
	//audio_playfile(&state->audio, AUDP_STARTUP, &state->semhnd_cmd);

	state->current_state = STATE_ACTIVE;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* PUSHBUTTON AND NOTIFICATIONS ----------------------------------------*/
/*----------------------------------------------------------------------*/

void smPushbuttonManage(mainState *state)
{
	if (!btClassicEnabled(&state->bluetooth))
		return;

	if (state->pb_laststate == powerui_pb_pollstate(&state->power))
		return;

	if (powerui_pb_pollstate(&state->power) == PB_LongPress_3s)
	{
		if (!btAudioEnabled(&state->bluetooth))
		{
			//Play Bluetooth ON audio
			audio_playfile(&state->audio, AUDP_BTON, &state->semhnd_cmd);

			btAudioControl(&state->bluetooth, true);
		}
		else
		{
			//Play Bluetooth OFF audio
			audio_playfile(&state->audio, AUDP_BTOFF, &state->semhnd_cmd);

			btAudioControl(&state->bluetooth, false);
		}
	}
	else if (powerui_pb_pollstate(&state->power) == PB_LongPress_6s)
	{
		if (btAudioEnabled(&state->bluetooth))
		{
			//Play Bluetooth PAIRING audio
			audio_playfile(&state->audio, AUDP_BTPAIR, &state->semhnd_cmd);

			btAudioPair(&state->bluetooth);
		}
	}
	else if (powerui_pb_pollstate(&state->power) == PB_LongPress_10s)
	{
		//Play Factory reset confirm audio clip
		audio_playfile(&state->audio, AUDP_FRCONFIRM, &state->semhnd_cmd);

	}
	else if (powerui_pb_pollstate(&state->power) == PB_LongPress_20s)
	{
		//Play Factory reset confirm audio clip
		audio_playfile(&state->audio, AUDP_FR, &state->semhnd_cmd);

		//Perform factory reset
		smFactoryReset(state);
	}
	else if (powerui_pb_pollstate(&state->power) == PB_SingleClick)
	{
		if (btIsAirplaneMode(&state->bluetooth))
		{
			btAirplaneMode(&state->bluetooth, false);
		}
		else if (btAudioEnabled(&state->bluetooth))
		{
			if (btIsPaired(&state->bluetooth))
			{
				btSendPlayPause(&state->bluetooth);
			}
		}
	}

	state->pb_laststate = powerui_pb_pollstate(&state->power);
}

/*----------------------------------------------------------------------*/

void smUserNotifications(mainState *state)
{
	if (!state->usernotification_enabled)
		return;

	smLEDNotifications(state);

	smAudioNotifications(state);
}

/*----------------------------------------------------------------------*/

void smLEDNotifications(mainState *state)
{

	if (btIsPairing(&state->bluetooth))
	{
		if (state->power.led_active_animation != LED_Anim_Pairing)
		{
			powerui_post_cmdwait(&state->power, LED_CMD_PAIR_START, &state->semhnd_cmd, 50);
		}
		return;
	}
	else if (state->power.led_active_animation == LED_Anim_Pairing)
	{
		powerui_post_cmdwait(&state->power, LED_CMD_OFF, &state->semhnd_cmd, 50);
	}

	if (powerui_bq_ChargingCheck(&state->power))
	{
		if (state->power.led_active_animation != LED_Anim_Charging)
		{
			powerui_post_cmdwait(&state->power, LED_CMD_CHARGE_START, &state->semhnd_cmd, 50);
		}
		return;
	}
	else if (state->power.led_active_animation == LED_Anim_Charging)
	{
		powerui_post_cmdwait(&state->power, LED_CMD_OFF, &state->semhnd_cmd, 50);
	}

	if (poweruiChkSOC(&state->power) < SM_BATLOW_PERCENT)
	{
		if (state->power.led_active_animation != LED_Anim_Lowbattery)
		{
			powerui_post_cmdwait(&state->power, LED_CMD_LOWBAT_START, &state->semhnd_cmd, 50);
		}
		return;
	}
	else if (state->power.led_active_animation == LED_Anim_Lowbattery)
	{
		powerui_post_cmdwait(&state->power, LED_CMD_OFF, &state->semhnd_cmd, 50);
	}
}

/*----------------------------------------------------------------------*/

void smAudioNotifications(mainState *state)
{
	if (poweruiChkSOC(&state->power) < SM_BATLOW_PERCENT)
	{
		if (poweruiChkSOC(&state->power) < SM_BATCRITICAL_PERCENT)
		{
			if (!state->audplayed_batcritical)
			{
				//Play Battery Critical audio clip
				audio_playfile(&state->audio, AUDP_BATCRITICAL, &state->semhnd_cmd);
			}
		}
		else
		{
			if (!state->audplayed_batlow)
			{
				//Play Battery Low audio clip
				audio_playfile(&state->audio, AUDP_BATLOW, &state->semhnd_cmd);
			}
		}
	}
	else
	{
		state->audplayed_batlow = false;
		state->audplayed_batcritical = false;
	}

	if (btIsPaired(&state->bluetooth))
	{
		if (!state->audplayed_paired)
		{
			//Play Bluetooth paired audio clip
			audio_playfile(&state->audio, AUDP_BTPAIRED, &state->semhnd_cmd);
		}
	}
	else
	{
		state->audplayed_paired = false;
	}
}

/*----------------------------------------------------------------------*/

void smEnableUserNotifications(mainState *state)
{
	state->usernotification_enabled = true;
}

/*----------------------------------------------------------------------*/

void smDisableUserNotifications(mainState *state)
{
	//Turn off LED notifications
	powerui_post_cmdwait(&state->power, LED_CMD_OFF, &state->semhnd_cmd, 50);

	state->usernotification_enabled = false;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* LOAD SWITCH ---------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void smLoadSwitchControl(mainState *state, bool enable)
{
	if (enable)
	{
		powerui_post_cmdwait(&state->power, PWR_CMD_LS_EN, &state->semhnd_cmd, 50);
	}
	else
	{
		powerui_post_cmdwait(&state->power, PWR_CMD_LS_DIS, &state->semhnd_cmd, 50);
	}

	//Wait for LSW output to rise
	rtos_Sleep_mS(50);
}

/*----------------------------------------------------------------------*/

bool smFactoryReset(mainState *state)
{
	MemElement elem;
	uint16_t elem_index;

	//Read from Factory Backup chunk and push data to relevant locations
	//System Details
	//Extract Element
	if (memReadElementParam(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_FACTORYRST_IND],
			&elem, SM_FACTORYRST_SYSDETAIL_ELEMIND) != MEM_OP_SUCCESS)
	   return false;

	if (memReadFromElement(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_FACTORYRST_IND],
		   &elem, 0, (void *)&state->details, sizeof(sysDetails)) != MEM_OP_SUCCESS)
		return false;

	//Erase System Details chunk
	if (memInitChunk(&state->peripherals.mem,
			&state->peripherals.mem.chunks[SM_MEMCHUNK_SYSDETAIL_IND]) != MEM_OP_SUCCESS)
		return false;

	//Write data
	if (memAddElement(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_SYSDETAIL_IND],
			sizeof(sysDetails), &elem, &elem_index)
			!= MEM_OP_SUCCESS)
		return false;

	if (memWriteToElement(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_SYSDETAIL_IND],
			&elem, 0, (void *)&state->details, sizeof(sysDetails)) != MEM_OP_SUCCESS)
		return false;

	//Audio Adjustments
	//Extract Element
	if (memReadElementParam(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_FACTORYRST_IND],
			&elem, SM_FACTORYRST_INVOL_ELEMIND) != MEM_OP_SUCCESS)
	   return false;

	if (memReadFromElement(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_FACTORYRST_IND],
		   &elem, 0, (void *)&state->audio.input_config, (sizeof(AudioInputConfig) * AUD_INPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
		return false;

	if (memReadElementParam(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_FACTORYRST_IND],
			&elem, SM_FACTORYRST_INVOLADJ_ELEMIND) != MEM_OP_SUCCESS)
	   return false;

	if (memReadFromElement(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_FACTORYRST_IND],
		   &elem, 0, (void *)&state->audio.input_adj,  (sizeof(int16_t) * AUD_INPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
		return false;

	if (memReadElementParam(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_FACTORYRST_IND],
			&elem, SM_FACTORYRST_OUTVOL_ELEMIND) != MEM_OP_SUCCESS)
	   return false;

	if (memReadFromElement(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_FACTORYRST_IND],
		   &elem, 0, (void *)&state->audio.output_config, (sizeof(AudioOutputConfig) * AUD_OUTPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
		return false;

	if (memReadElementParam(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_FACTORYRST_IND],
			&elem, SM_FACTORYRST_OUTVOLADJ_ELEMIND) != MEM_OP_SUCCESS)
	   return false;

	if (memReadFromElement(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_FACTORYRST_IND],
		   &elem, 0, (void *)&state->audio.output_adj,  (sizeof(int16_t) * AUD_OUTPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
		return false;

	if (memReadElementParam(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_FACTORYRST_IND],
			&elem, SM_FACTORYRST_EQ_ELEMIND) != MEM_OP_SUCCESS)
	   return false;

	if (memReadFromElement(&state->peripherals.mem, &state->peripherals.mem.chunks[SM_MEMCHUNK_FACTORYRST_IND],
		   &elem, 0, (void *)&state->audio.eq_config,  (sizeof(AudioEQConfig) * AUD_EQ_NUM_CHANNELS)) != MEM_OP_SUCCESS)
		return false;

	//Write data
	if (memAddElement(&state->peripherals.mem, &state->peripherals.mem.chunks[AUD_MEMCHUNK_ADJ_IND],
			(sizeof(AudioInputConfig) * AUD_INPUT_NUM_CHANNELS), &elem, &elem_index)
			!= MEM_OP_SUCCESS)
		return false;

	if (memWriteToElement(&state->peripherals.mem, &state->peripherals.mem.chunks[AUD_MEMCHUNK_ADJ_IND],
			&elem, 0, (void *)&state->audio.input_config, (sizeof(AudioInputConfig) * AUD_INPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
		return false;

	if (memAddElement(&state->peripherals.mem, &state->peripherals.mem.chunks[AUD_MEMCHUNK_ADJ_IND],
			(sizeof(int16_t) * AUD_INPUT_NUM_CHANNELS), &elem, &elem_index)
			!= MEM_OP_SUCCESS)
		return false;

	if (memWriteToElement(&state->peripherals.mem, &state->peripherals.mem.chunks[AUD_MEMCHUNK_ADJ_IND],
			&elem, 0, (void *)&state->audio.input_adj, (sizeof(int16_t) * AUD_INPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
		return false;

	if (memAddElement(&state->peripherals.mem, &state->peripherals.mem.chunks[AUD_MEMCHUNK_ADJ_IND],
			(sizeof(AudioOutputConfig) * AUD_OUTPUT_NUM_CHANNELS), &elem, &elem_index)
			!= MEM_OP_SUCCESS)
		return false;

	if (memWriteToElement(&state->peripherals.mem, &state->peripherals.mem.chunks[AUD_MEMCHUNK_ADJ_IND],
			&elem, 0, (void *)&state->audio.output_config, (sizeof(AudioOutputConfig) * AUD_OUTPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
		return false;

	if (memAddElement(&state->peripherals.mem, &state->peripherals.mem.chunks[AUD_MEMCHUNK_ADJ_IND],
			(sizeof(int16_t) * AUD_OUTPUT_NUM_CHANNELS), &elem, &elem_index)
			!= MEM_OP_SUCCESS)
		return false;

	if (memWriteToElement(&state->peripherals.mem, &state->peripherals.mem.chunks[AUD_MEMCHUNK_ADJ_IND],
			&elem, 0, (void *)&state->audio.output_adj, (sizeof(int16_t) * AUD_OUTPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
		return false;

	if (memAddElement(&state->peripherals.mem, &state->peripherals.mem.chunks[AUD_MEMCHUNK_ADJ_IND],
			(sizeof(AudioEQConfig) * AUD_EQ_NUM_CHANNELS), &elem, &elem_index)
			!= MEM_OP_SUCCESS)
		return false;

	if (memWriteToElement(&state->peripherals.mem, &state->peripherals.mem.chunks[AUD_MEMCHUNK_ADJ_IND],
			&elem, 0, (void *)&state->audio.eq_config, (sizeof(AudioEQConfig) * AUD_EQ_NUM_CHANNELS)) != MEM_OP_SUCCESS)
		return false;

	//Reset
	smQueueSoftReset(state);

	return true;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* LOW POWER MANAGEMENT ------------------------------------------------*/
/*----------------------------------------------------------------------*/

void smSetLowPowerSleep(mainState *state)
{
	state->lowpower_mode = LP_SLEEP;
}

/*----------------------------------------------------------------------*/

void smSetLowPowerStop(mainState *state)
{
	state->lowpower_mode = LP_STOP;
}

/*----------------------------------------------------------------------*/

void smEnterSLEEPMode(mainState *state)
{
	__HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_EVENT();
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_DBGMCU_EnableDBGSleepMode();
}

/*----------------------------------------------------------------------*/

void smExitSLEEPMode(mainState *state)
{
	//Nothing to reset
}

/*----------------------------------------------------------------------*/

void smEnterSTOPMode(mainState *state)
{
	//Go into STOP
	__HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
	__HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_EVENT();
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_DBGMCU_EnableDBGStopMode();

	//Put unused GPIOs into Analog mode
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	//GPIOA
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3			//EXT USART2
			|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
			|GPIO_PIN_11|GPIO_PIN_12 					//BT Module USART1
			|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7			//MEM SPI1
			|GPIO_PIN_15;								//I2S3 WCLK

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//GPIOB
	GPIO_InitStruct.Pin = PM_INT_Pin			//PMIC inputs
			|GPIO_PIN_6|GPIO_PIN_7						//PMIC I2C1
			|EN_1V2_Pin									//1V2_EN (only safe when 1V8_aud is LOW)
			|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
			|GPIO_PIN_15								//SPI2 DSP
			|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;			//I2S3 BCLK DI DO

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/*----------------------------------------------------------------------*/

void smExitSTOPMode(mainState *state)
{
	XTALClock_Config();

	//Reconfigure GPIOs
	MX_GPIO_Init();
	HAL_I2C_MspInit(&hi2c1);
	HAL_I2S_MspInit(&hi2s3);
	HAL_SPI_MspInit(&hspi1);
	HAL_SPI_MspInit(&hspi2);
	HAL_UART_MspInit(&huart2);

	HAL_UART_MspInit(&huart1);
}

/*----------------------------------------------------------------------*/

void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
	return;//NOTE: Skip sleep
	//If any of our peripherals are currently active, DO NOT SLEEP
	if (i2cCheckBusy(&main_state.peripherals.i2c) ||
			spiCheckBusy(&main_state.peripherals.spi) ||
			i2sCheckBusy(&main_state.peripherals.i2s) ||
			memCheckBusy(&main_state.peripherals.mem))
		return;

	/* Stop the timer that is generating the tick interrupt. */
	HAL_SuspendTick();

	HAL_PWREx_EnableFlashPowerDown();
	HAL_PWREx_EnableMainRegulatorLowVoltage();

	/* Enter a critical section but don't use the taskENTER_CRITICAL()
			method as that will mask interrupts that should exit sleep mode. */
	__asm volatile( "cpsid i" ::: "memory" );
	__asm volatile( "dsb" );
	__asm volatile( "isb" );

	RTC_TimeTypeDef time_before, time_after;
	RTC_DateTypeDef date;
	HAL_RTC_GetTime(&hrtc, &time_before, RTC_FORMAT_BIN);

	int32_t ticks_slept = time_before.SubSeconds;

	/* Ensure it is still ok to enter the sleep mode. */
	eSleepModeStatus eSleepStatus = eTaskConfirmSleepModeStatus();

	if( eSleepStatus != eAbortSleep )
	{
		//Set up RTC to wake up in xExpectedIdleTime
		uint32_t rtc_count = xExpectedIdleTime * 2;
		if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, rtc_count, RTC_WAKEUPCLOCK_RTCCLK_DIV16) == HAL_OK)
		{
			if (main_state.lowpower_mode == LP_SLEEP)
			{
				//Go into SLEEP
				smEnterSLEEPMode(&main_state);
				HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE);
				smExitSLEEPMode(&main_state);
			}
			else if (main_state.lowpower_mode == LP_STOP)
			{
				smEnterSTOPMode(&main_state);
				HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFE);
				smExitSTOPMode(&main_state);

			}

			//Turn off auto wakeup
			HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

			//Wake up here
			//Find the amount of time we have slept in ticks
			HAL_RTC_GetTime(&hrtc, &time_after, RTC_FORMAT_BIN);

			ticks_slept -= time_after.SubSeconds;

			if (ticks_slept < 0)
				ticks_slept = (int32_t)time_after.SecondFraction + 1 + ticks_slept;

			if (ticks_slept > xExpectedIdleTime)
				ticks_slept = xExpectedIdleTime;

			HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);		//Only done to unlock current date

			//Correct number of ticks
			if (ticks_slept > 0)
				vTaskStepTick((TickType_t)ticks_slept);
		}
	}

	/* Re-enable interrupts - see comments above the cpsid instruction()
	above. */
	__asm volatile( "cpsie i" ::: "memory" );
	__asm volatile( "dsb" );
	__asm volatile( "isb" );

	/* Restart SysTick. */
	HAL_ResumeTick();
}

