/*! \file "btDevCtrl.c"
   \brief Contains handler functions for all characteristics in
   "Device Control" service

   This service is used to execute device processes.
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "btDevCtrl.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Handles incoming data for Characteristic btDevCtrl/VolumeSet

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlVolumeSetHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/DeviceIDSet

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlDeviceIDSetHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/DeviceCalibration

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlDeviceCalibration1Handler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/SleepControl

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlSleepControlHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/AirplaneMode

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlAirplaneModeHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/EQControl

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlEQControlHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/AIControl

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlAIControlHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/SpeechControl

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlSpeechControlHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/SmartBatteryControl

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlSmartBatteryControlHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/FactoryReset

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlFactoryResetHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/UsageStat

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlUsageStatHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/CalibrationMode

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlCalibrationModeHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/DeviceInfo

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlDeviceInfoHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/EQPoll

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlEQPollHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/VolumePoll

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlVolumePollHandler(mainState *state, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btDevCtrl/SetPairing

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btDevCtrlStatus btDevCtrlSetPairingHandler(mainState *state, uint8_t *pCharData);

/*! \brief Extract the message type

    \param *pCharData Incoming data
	\param *type_return Type of message
*/
static void btDevCtrlUtilExtractMsgType(uint8_t *pCharData, uint8_t *type_return);

/*! \brief Transmit back to master

	\param Interface used in communication
    \param param Profile parameter ID
    \param len length of data to write
    \param *value pointer to data to write.  This is dependent on
             the parameter ID and may be cast to the appropriate
             data type (example: data type of uint16_t will be cast to
             uint16_t pointer).

    \return Success
*/
static bool DevCtrlService_TX(btDevCtrlInterface interface, uint8_t param, uint16_t len, void *value);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* LOCAL VARIABLES -----------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \var btDevCtrlState ops_state
  Contains the state of this service.
  Used to keep track of multi-message transfers
*/
btDevCtrlState *ops_state;

btDevCtrlState ops_state_uart;
btDevCtrlState ops_state_ble;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* DEVICE CONTROL TX FUNCTION ------------------------------------------*/
/*----------------------------------------------------------------------*/

bool DevCtrlService_TX(btDevCtrlInterface interface, uint8_t param, uint16_t len, void *value)
{
	if (interface == DEVCTRL_UART)
		return DevCtrlService_SetParameter_UART(param, len, value);
	else if (interface == DEVCTRL_BLE)
		return false;
//		return DevCtrlService_SetParameter_BLE(param, len, value);
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* DEVICE CONTROL CHARACTERISTIC HANDLER FUNCTIONS ---------------------*/
/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlService_Handler(btDevCtrlInterface interface, mainState *state,
		uint8_t *pCharData, uint8_t charId)
{
	if (interface == DEVCTRL_UART)
		ops_state = &ops_state_uart;
	else if (interface == DEVCTRL_BLE)
		ops_state = &ops_state_ble;

	ops_state->interface = interface;

    switch (charId)
    {
    case BTDEV_VOLSET_ID:
        return btDevCtrlVolumeSetHandler(state, pCharData);

    case BTDEV_DEVIDSET_ID:
        return btDevCtrlDeviceIDSetHandler(state, pCharData);

    case BTDEV_DEVCALIB1_ID:
        return btDevCtrlDeviceCalibration1Handler(state, pCharData);

    case BTDEV_SLEEPCTRL_ID:
        return btDevCtrlSleepControlHandler(state, pCharData);

    case BTDEV_AIRPLANE_ID:
    	return btDevCtrlAirplaneModeHandler(state, pCharData);

    case BTDEV_EQCTRL_ID:
    	return btDevCtrlEQControlHandler(state, pCharData);

    case BTDEV_AICTRL_ID:
    	return btDevCtrlAIControlHandler(state, pCharData);

    case BTDEV_SPEECHCTRL_ID:
    	return btDevCtrlSpeechControlHandler(state, pCharData);

    case BTDEV_SMARTBATTCTRL_ID:
    	return btDevCtrlSmartBatteryControlHandler(state, pCharData);

    case BTDEV_FACTORYRST_ID:
    	return btDevCtrlFactoryResetHandler(state, pCharData);

    case BTDEV_USAGESTAT_ID:
    	return btDevCtrlUsageStatHandler(state, pCharData);

    case BTDEV_CALIBMODE_ID:
    	return btDevCtrlCalibrationModeHandler(state, pCharData);

    case BTDEV_DEVINFO_ID:
    	return btDevCtrlDeviceInfoHandler(state, pCharData);

    case BTDEV_EQPOLL_ID:
    	return btDevCtrlEQPollHandler(state, pCharData);

    case BTDEV_VOLPOLL_ID:
    	return btDevCtrlVolumePollHandler(state, pCharData);

    case BTDEV_SETPAIR_ID:
        	return btDevCtrlSetPairingHandler(state, pCharData);

    default:
      return DEVCTRL_STAT_FAIL;
    }
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlVolumeSetHandler(mainState *state, uint8_t *pCharData)
{
    //Extract message type
    uint8_t msg_type;
    btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

    //Filter types
    if (msg_type !=DEVCTRL_MSGTYPES_VOLSET)
        return DEVCTRL_STAT_INVALIDTYPE;

    int8_t vol, ch;
    memcpy(&vol, pCharData+1, sizeof(uint8_t));
    memcpy(&ch, pCharData+2, sizeof(uint8_t));

    int16_t volume = ((vol * (AUD_OUTVOL_DBCEIL + (-1 * AUD_OUTVOL_DBFLOOR))) / 100) - abs(AUD_OUTVOL_DBFLOOR);

    uint8_t resp[BTDEV_VOLSET_LEN];
    resp[0] = 2;
    memcpy(resp+2, &vol, sizeof(uint8_t));
    memcpy(resp+3, &ch, sizeof(uint8_t));

    switch (msg_type)
    {
    case 1:
        if (ch >= AUD_OUTPUT_NUM_CHANNELS)
        {
            resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_CHANNELINVALID;

            DevCtrlService_TX(ops_state->interface, BTDEV_VOLSET_ID, 4, resp);
            return DEVCTRL_STAT_INVALIDPARAM;
        }
        else if (volume > AUD_INVOL_MAXDB || volume < AUD_INVOL_MINDB)
        {
            resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_VALUEINVALID;
            DevCtrlService_TX(ops_state->interface, BTDEV_VOLSET_ID, 4, resp);
            return DEVCTRL_STAT_INVALIDPARAM;
        }
        else
        {
            resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_SUCCESS;
            DevCtrlService_TX(ops_state->interface, BTDEV_VOLSET_ID, 4, resp);

            audio_Output_VolumeSet(&state->audio, ch, volume);

            return DEVCTRL_STAT_SUCCESS;
        }
    default:
        return DEVCTRL_STAT_INVALIDTYPE;
    }
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlDeviceIDSetHandler(mainState *state, uint8_t *pCharData)
{
    //Extract message type
    uint8_t msg_type;
    btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

    //Filter types
    if (msg_type !=DEVCTRL_MSGTYPES_DEVIDSET)
        return DEVCTRL_STAT_INVALIDTYPE;

    char devid[4];
    char devName[13];
    char model[2];
    memcpy(devid, pCharData+1, 4);
    memcpy(devName, pCharData+5, 13);
    memcpy(model, pCharData+18, 2);

    uint8_t resp[BTDEV_DEVIDSET_LEN];
    resp[0] = 2;
    memcpy(resp+2, devid, 4);
    memcpy(resp+6, devName, 13);
    memcpy(resp+19, model, 2);

    switch (msg_type)
    {
    case 1:
    	//TODO: Check if DevID DevName and Model are valid

    	resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_SUCCESS;
		DevCtrlService_TX(ops_state->interface, BTDEV_DEVIDSET_ID, 21, resp);

        return DEVCTRL_STAT_SUCCESS;
    default:
        return DEVCTRL_STAT_INVALIDTYPE;
    }
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlDeviceCalibration1Handler(mainState *state, uint8_t *pCharData)
{
    //Extract message type
    uint8_t msg_type;
    btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

    //Filter types
    if (msg_type !=DEVCTRL_MSGTYPES_DEVCALIB1)
        return DEVCTRL_STAT_INVALIDTYPE;

    int16_t adj[4];
    uint8_t resp[BTDEV_DEVCALIB1_LEN];

    for (uint8_t i = 0; i < 4; i++)
    {
		memcpy(&adj[i], pCharData+1+(i*2), sizeof(int16_t));
		memcpy(resp+2+(i*2), pCharData+1+(i*2), sizeof(int16_t));
    }

    resp[0] = 2;

    switch (msg_type)
    {
    case 1:
    	for (uint8_t i = 0; i < 4; i++)
    	{
    		if (adj[i] < -100 || adj[i] > 100)
    		{
    			resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_VALUEINVALID;
				DevCtrlService_TX(ops_state->interface, BTDEV_DEVCALIB1_ID, 10, resp);
				return DEVCTRL_STAT_INVALIDPARAM;
    		}
    	}

        state->audio.output_adj[0] = adj[0];
        state->audio.output_adj[1] = adj[1];
        state->audio.input_adj[0] = adj[2];
        state->audio.input_adj[2] = adj[3];

        resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_SUCCESS;
        DevCtrlService_TX(ops_state->interface, BTDEV_DEVCALIB1_ID, 10, resp);

        return DEVCTRL_STAT_SUCCESS;
    default:
        return DEVCTRL_STAT_INVALIDTYPE;
    }
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlSleepControlHandler(mainState *state, uint8_t *pCharData)
{
    //Extract message type
    uint8_t msg_type;
    btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

    //Filter types
    if (msg_type !=DEVCTRL_MSGTYPES_SLEEPCTRL)
        return DEVCTRL_STAT_INVALIDTYPE;

    uint8_t onoff;
    memcpy(&onoff, pCharData+1, sizeof(onoff));

    uint8_t resp[BTDEV_SLEEPCTRL_LEN];
    resp[0] = 2;

    switch (msg_type)
    {
    case 1:
        resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_SUCCESS;
        DevCtrlService_TX(ops_state->interface, BTDEV_SLEEPCTRL_ID, 2, resp);

        if (onoff == 1)
        	smQueueSleep(state);
        else
        	smQueueWake(state);

        return DEVCTRL_STAT_SUCCESS;
    default:
        return DEVCTRL_STAT_INVALIDTYPE;
    }
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlAirplaneModeHandler(mainState *state, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type !=DEVCTRL_MSGTYPES_AIRPLANE)
		return DEVCTRL_STAT_INVALIDTYPE;

	uint8_t resp[BTDEV_AIRPLANE_LEN];
	resp[0] = 2;

	switch (msg_type)
	{
	case 1:
		resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_SUCCESS;
		DevCtrlService_TX(ops_state->interface, BTDEV_DEVCALIB1_ID, 2, resp);

		btAirplaneMode(&state->bluetooth, true);

		return DEVCTRL_STAT_SUCCESS;
	default:
		return DEVCTRL_STAT_INVALIDTYPE;
	}
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlEQControlHandler(mainState *state, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type !=DEVCTRL_MSGTYPES_EQCTRL)
		return DEVCTRL_STAT_INVALIDTYPE;

	uint8_t channel;
	memcpy(&channel, pCharData+1, sizeof(channel));

	for (uint8_t i = 0; i < 5; i++)
	{
		memcpy(&ops_state->eq_bands[(msg_type*5) + i], pCharData+2+(i*2), sizeof(int16_t));
	}

	uint8_t resp[BTDEV_EQCTRL_LEN];
	resp[0] = 3;


	switch (msg_type)
	{
	case 2:
		if (channel > 3)
		{
			resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_CHANNELINVALID;
			DevCtrlService_TX(ops_state->interface, BTDEV_EQCTRL_ID, 2, resp);
			return DEVCTRL_STAT_INVALIDPARAM;
		}

		for (uint8_t i = 0; i < 10; i++)
		{
			if (ops_state->eq_bands[i] < -400 || ops_state->eq_bands[i] > 400)
			{
				resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_VALUEINVALID;
				DevCtrlService_TX(ops_state->interface, BTDEV_EQCTRL_ID, 2, resp);
				return DEVCTRL_STAT_INVALIDPARAM;
			}
		}

		audio_EQ_Set(&state->audio, channel, ops_state->eq_bands);
		resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_SUCCESS;

		DevCtrlService_TX(ops_state->interface, BTDEV_EQCTRL_ID, 2, resp);

		return DEVCTRL_STAT_SUCCESS;
	default:
		return DEVCTRL_STAT_INVALIDTYPE;
	}
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlAIControlHandler(mainState *state, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type !=DEVCTRL_MSGTYPES_AICTRL)
		return DEVCTRL_STAT_INVALIDTYPE;

	uint8_t onoff;
	memcpy(&onoff, pCharData+1, sizeof(onoff));

	uint8_t resp[BTDEV_AICTRL_LEN];
	resp[0] = 2;

	switch (msg_type)
	{
	case 1:
		//TODO populate once feature created
		resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_PROCESSFAILED;
		DevCtrlService_TX(ops_state->interface, BTDEV_AICTRL_ID, 2, resp);
		return DEVCTRL_STAT_SUCCESS;
	default:
		return DEVCTRL_STAT_INVALIDTYPE;
	}
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlSpeechControlHandler(mainState *state, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type !=DEVCTRL_MSGTYPES_SPCHCTRL)
		return DEVCTRL_STAT_INVALIDTYPE;

	uint8_t onoff;
	memcpy(&onoff, pCharData+1, sizeof(onoff));

	uint8_t resp[BTDEV_SPEECHCTRL_LEN];
	resp[0] = 2;

	switch (msg_type)
	{
	case 1:
		//TODO populate once feature created
		resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_PROCESSFAILED;
		DevCtrlService_TX(ops_state->interface, BTDEV_SPEECHCTRL_ID, 2, resp);
		return DEVCTRL_STAT_SUCCESS;
	default:
		return DEVCTRL_STAT_INVALIDTYPE;
	}
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlSmartBatteryControlHandler(mainState *state, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type !=DEVCTRL_MSGTYPES_SMARTBATCTRL)
		return DEVCTRL_STAT_INVALIDTYPE;

	uint8_t onoff;
	memcpy(&onoff, pCharData+1, sizeof(onoff));

	uint8_t resp[BTDEV_SMARTBATTCTRL_LEN];
	resp[0] = 2;

	switch (msg_type)
	{
	case 1:
		//TODO populate once feature created
		resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_PROCESSFAILED;
		DevCtrlService_TX(ops_state->interface, BTDEV_SMARTBATTCTRL_ID, 2, resp);
		return DEVCTRL_STAT_SUCCESS;
	default:
		return DEVCTRL_STAT_INVALIDTYPE;
	}
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlFactoryResetHandler(mainState *state, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type !=DEVCTRL_MSGTYPES_FACTORYRESET)
		return DEVCTRL_STAT_INVALIDTYPE;

	uint8_t resp[BTDEV_FACTORYRST_LEN];
	resp[0] = 2;

	switch (msg_type)
	{
	case 1:
		//Perform factory reset
		if (!smFactoryReset(state))
		{
			resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_PROCESSFAILED;
			DevCtrlService_TX(ops_state->interface, BTDEV_FACTORYRST_ID, 2, resp);
		}
		else
		{
			resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_SUCCESS;
			DevCtrlService_TX(ops_state->interface, BTDEV_FACTORYRST_ID, 2, resp);
		}


		return DEVCTRL_STAT_SUCCESS;
	default:
		return DEVCTRL_STAT_INVALIDTYPE;
	}
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlUsageStatHandler(mainState *state, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type !=DEVCTRL_MSGTYPES_USAGESTAT)
		return DEVCTRL_STAT_INVALIDTYPE;

	uint16_t min_today, min_yesterday;
	memcpy(&min_today, pCharData+1, sizeof(min_today));
	memcpy(&min_yesterday, pCharData+3, sizeof(min_yesterday));

	uint8_t resp[BTDEV_USAGESTAT_LEN];
	resp[0] = 2;

	switch (msg_type)
	{
	case 1:
		//TODO: Define protocol
		return DEVCTRL_STAT_SUCCESS;
	default:
		return DEVCTRL_STAT_INVALIDTYPE;
	}
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlCalibrationModeHandler(mainState *state, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type !=DEVCTRL_MSGTYPES_CALIBMODE)
		return DEVCTRL_STAT_INVALIDTYPE;

	uint8_t resp[BTDEV_CALIBMODE_LEN];

	switch (msg_type)
	{
	case 1:
		//TODO: Define protocol

		return DEVCTRL_STAT_SUCCESS;

	case 3:
		//TODO: Define protocol

		return DEVCTRL_STAT_SUCCESS;

	default:
		return DEVCTRL_STAT_INVALIDTYPE;
	}
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlDeviceInfoHandler(mainState *state, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type !=DEVCTRL_MSGTYPES_DEVINFO)
		return DEVCTRL_STAT_INVALIDTYPE;

	uint8_t resp[BTDEV_DEVINFO_LEN];
	resp[0] = 2;

	switch (msg_type)
	{
	case 1:
		resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_SUCCESS;
		memcpy(state->details.devID, resp+2, 4);
		memcpy(state->details.devName, resp+6, 13);
		memcpy(state->details.model, resp+19, 2);

		DevCtrlService_TX(ops_state->interface, BTDEV_DEVINFO_ID, 21, resp);

		rtos_Sleep_mS(100);

		resp[0] = 3;
		resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_SUCCESS;
		memcpy(state->details.fw_version, resp+2, 1);
		if (state->details.calibrated)
			resp[3] = 1;
		else
			resp[3] = 0;
		if(powerui_bq_ChargingCheck(&state->power))
			resp[4] = 0;
		else
			resp[4] = 3;

		DevCtrlService_TX(ops_state->interface, BTDEV_DEVINFO_ID, 5, resp);

		return DEVCTRL_STAT_SUCCESS;
	default:
		return DEVCTRL_STAT_INVALIDTYPE;
	}

	return DEVCTRL_STAT_SUCCESS;
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlEQPollHandler(mainState *state, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type !=DEVCTRL_MSGTYPES_EQPOLL)
		return DEVCTRL_STAT_INVALIDTYPE;

	uint8_t channel;
	memcpy(&channel, pCharData+1, sizeof(channel));

	uint8_t resp[BTDEV_EQPOLL_LEN];
	resp[0] = 2;

	switch (msg_type)
	{
	case 1:
		if (channel > 3)
		{
			resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_CHANNELINVALID;
			DevCtrlService_TX(ops_state->interface, BTDEV_EQPOLL_ID, 2, resp);
			return DEVCTRL_STAT_INVALIDPARAM;
		}

		resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_SUCCESS;
		resp[2] = channel;
		memcpy(resp+3, &state->audio.eq_config[channel].band[0], sizeof(int16_t));
		memcpy(resp+5, &state->audio.eq_config[channel].band[1], sizeof(int16_t));
		memcpy(resp+7, &state->audio.eq_config[channel].band[2], sizeof(int16_t));
		memcpy(resp+9, &state->audio.eq_config[channel].band[3], sizeof(int16_t));
		memcpy(resp+11, &state->audio.eq_config[channel].band[4], sizeof(int16_t));
		DevCtrlService_TX(ops_state->interface, BTDEV_EQPOLL_ID, 13, resp);

		rtos_Sleep_mS(100);

		resp[0] = 3;
		memcpy(resp+3, &state->audio.eq_config[channel].band[5], sizeof(int16_t));
		memcpy(resp+5, &state->audio.eq_config[channel].band[6], sizeof(int16_t));
		memcpy(resp+7, &state->audio.eq_config[channel].band[7], sizeof(int16_t));
		memcpy(resp+9, &state->audio.eq_config[channel].band[8], sizeof(int16_t));
		memcpy(resp+11, &state->audio.eq_config[channel].band[9], sizeof(int16_t));
		DevCtrlService_TX(ops_state->interface, BTDEV_EQPOLL_ID, 13, resp);

		return DEVCTRL_STAT_SUCCESS;
	default:
		return DEVCTRL_STAT_INVALIDTYPE;
	}

	return DEVCTRL_STAT_SUCCESS;
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlVolumePollHandler(mainState *state, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type !=DEVCTRL_MSGTYPES_VOLPOLL)
		return DEVCTRL_STAT_INVALIDTYPE;

	int8_t channel;
	memcpy(&channel, pCharData+1, sizeof(uint8_t));

    int16_t volume = 0;

    uint8_t resp[BTDEV_VOLPOLL_LEN];
	resp[0] = 2;

	switch (msg_type)
	{
	case 1:
		if (channel > 3)
		{
			resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_CHANNELINVALID;
			DevCtrlService_TX(ops_state->interface, BTDEV_VOLPOLL_ID, 2, resp);
			return DEVCTRL_STAT_INVALIDPARAM;
		}

		if (state->audio.output_config[channel].mute)
			volume = 0;
		else
		{
			volume = state->audio.output_config[channel].volume;
			volume = (volume * 100) / AUD_INVOL_MAXDB;
		}

		resp[2] = (int8_t)volume;
		resp[3] = channel;

		resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_SUCCESS;
		DevCtrlService_TX(ops_state->interface, BTDEV_VOLPOLL_ID, 4, resp);

		return DEVCTRL_STAT_SUCCESS;
	default:
		return DEVCTRL_STAT_INVALIDTYPE;
	}
}

/*----------------------------------------------------------------------*/

btDevCtrlStatus btDevCtrlSetPairingHandler(mainState *state, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btDevCtrlUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type !=DEVCTRL_MSGTYPES_VOLPOLL)
		return DEVCTRL_STAT_INVALIDTYPE;

	uint8_t resp[BTDEV_SETPAIR_LEN];
	resp[0] = 2;

	switch (msg_type)
	{
	case 1:
		resp[DEVCTRL_ALL_ACKERR_POS] = DEVCTRL_RESP_SUCCESS;
		DevCtrlService_TX(ops_state->interface, BTDEV_SETPAIR_ID, 2, resp);

		btAudioPair(&state->bluetooth);
		return DEVCTRL_STAT_SUCCESS;
	default:
		return DEVCTRL_STAT_INVALIDTYPE;
	}

	return DEVCTRL_STAT_SUCCESS;
}

/*----------------------------------------------------------------------*/

void btDevCtrlUtilExtractMsgType(uint8_t *pCharData, uint8_t *type_return)
{
    *type_return = pCharData[0];
}

