/*! \file "audio.c"
   \brief Main Audio subsystem file

    Contains top level functions for initialization and sleep of audio subsystem based
    on the CS47L90
    Also contains functions for volume adjustment at the input and output
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "audio.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Reset DSP memory and load program/parameters into CS47L90
 * (all enabled cores)

    \param *state Pointer to Audio object
    \return Pass or fail
*/
static bool audio_init_DSPs(AudioState *state);

/*! \brief Startup specified core.

    \param *state Pointer to Audio object
    \param core Core to start
    \return Pass or fail
*/
static bool audio_start_DSP(AudioState *state, uint32_t core);

/*! \brief Stop specified DSP

    \param *state Pointer to Audio object
    \param core Core to Stop
    \return Pass or fail
*/
static bool audio_stop_DSP(AudioState *state, uint32_t core);

/*! \brief Initialize CS Pin

    \param *state Pointer to Audio object
    \return Pass or fail
    \remark This is required due to the fact that the CS pin coming out of tri-state
    	seems to feed the CS47L90 1V8 rail, which in turn enables the 1V2 rail. This is our work
    	around. Only initialize the CS as an output until you are ready to power the CS47 chip.
*/
static void audio_init_SPICS(AudioState *state);

/*! \brief De-Initialize CS Pin

    \param *state Pointer to Audio object
    \return Pass or fail
    \remark Only initialize the CS as an output until you are ready to power the CS47 chip.
*/
static void audio_deinit_SPICS(AudioState *state);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* SYSTEM CONTROL ------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void audioInit(AudioState *state, PowerUIState *pwr, PeriObject *peri,
		GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    //Peripheral pointer
    state->peri = peri;

    //Command management
    rtos_createSemaphore(&state->sem_cmd, 1, 0);

    //Setup SPI
    spiInitTransaction(&state->spi_trans,
                       (void *)state->peri->mem.buffer,
                       (void *)state->peri->mem.buffer + (CS_SPI_RSIZE * sizeof(uint16_t)));

    //Save power object
    state->pwr = pwr;

    //CS pin control
    state->cs_port = cs_port;
    state->cs_pin = cs_pin;

    //Set audio I/O configurations
    state->dsp_config = audio_dsps;
    state->input_config = audio_inputs;
    state->output_config = audio_outputs;
    state->input_adj = audio_inadj;
    state->output_adj = audio_outadj;
    state->eq_config = audio_eq;

    //Initialize status
    state->status = CS_Sleeping;
}

/*----------------------------------------------------------------------*/

bool audio_cs_init(AudioState *state)
{
    //If we're not sleeping. Just bail
    if(state->status != CS_Sleeping)
        return false;

    state->status = CS_Initializing;

    //Turn on 1V8_aud and 1V2_aud
    if (!powerui_post_cmdwait(state->pwr, PWR_CMD_LSLDO_EN, &state->sem_cmd, 500))
        return false;

    //Start I2S stream (Enables BCLK for System, DSP and ASYNC clock sources)
	i2s_startEmptyStream(&state->peri->i2s);

    //Initialize the CS pin
    audio_init_SPICS(state);

    rtos_Sleep_mS(200);        //Give voltage rail a chance to rise and CODEC a chance to wake up
#ifndef AUD_EXTSPI
    //Load enabled defaults into Memory. Enabled/Disabled in symbols
    audioLoadDefaultsToNVM(state);

    //Perform software reset
    uint8_t elem_index = 0;
    audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], elem_index);
    rtos_Sleep_mS(20);
    elem_index++;
    audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], elem_index);

    //Check if boot sequence complete //Is this necessary? //Commented out for now
    /*if (!audio_read_address(state, AUD_IRQ1_STAT1, &state->spi_read))
        return false;
    if (((state->spi_read & 0x80) >> 7) != AUD_BOOT_DONE)
        return false;*/

    //Disable/Configure/Enable clocks
    elem_index = 2;
    audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], elem_index);
    elem_index++;
    audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], elem_index);
    rtos_Sleep_mS(200);      //Give FLL a chance to establish
    elem_index++;
    audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], elem_index);

	//Load CODEC Config
    elem_index = 5;
    while (audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], elem_index))
        elem_index++;

    //Load Adjustable parameters to RAM
    audioNVSToRAMLoadDSPConfig(state);
    audioNVSToRAMLoadInputConfig(state);
    audioNVSToRAMLoadOutputConfig(state);
    audioNVSToRAMLoadEQConfig(state);

    //Load Adjustable parameters to DSP
    //Input
    bool update = false;
    for (uint8_t i = 0; i < AUD_INPUT_NUM_CHANNELS; i++)
    {
        if (i == AUD_INPUT_NUM_CHANNELS - 1)
            update = true;
        if(!audioRAMToDSPLoadInputConfig(state, i, update))
            return false;
    }
    //Output
    update = false;
    for (uint8_t i = 0; i < AUD_OUTPUT_NUM_CHANNELS; i++)
    {
        if (i == AUD_OUTPUT_NUM_CHANNELS - 1)
            update = true;
        if(!audioRAMToDSPLoadOutputConfig(state, i, update))
            return false;
    }
    for (uint8_t i = 0; i < AUD_EQ_NUM_CHANNELS; i++)
	{
		if (i == AUD_EQ_NUM_CHANNELS - 1)
			update = true;
		if(!audioRAMToDSPLoadEQConfig(state, i, update))
			return false;
	}
    //EQ
    update = false;


    state->adj_updated = false;

    //Initialise and Startup DSP Cores
    //TODO: Reinstate
   /* bool dspinit_success = true;
    if (audio_init_DSPs(state))
    {
        //Start DSP cores
        for (uint8_t core = 0; core < AUDIO_DSP_CORES_MAX; core++)
        {
            if (!audio_start_DSP(state, core))
            {
                dspinit_success = false;
                break;
            }
        }
    }
    else
        dspinit_success = false;

    if (!dspinit_success)
        return false;*/
#endif

    //Update audio system status
    state->status = CS_Operating;

    return true;
}

/*----------------------------------------------------------------------*/

bool audio_cs_slpenter(AudioState *state)
{
#ifndef AUD_EXTSPI
    //Stop DSP cores
    for (uint8_t core = 0; core < AUDIO_DSP_CORES_MAX; core++)
        audio_stop_DSP(state, core);

    //Disable System clocks
    //Update System Clock 1 register
    if (!audio_write_address(state, CS_ADDR_SYSCLK1, CS_SYSCLK_DIS))
        return false;
    //Update DSP Clock 1 register
    if (!audio_write_address(state, CS_ADDR_DSPCLK1, CS_DSPCLK_DIS))
        return false;
    //Update Clock 32K 1 register
    if (!audio_write_address(state, CS_ADDR_32KCLK1, CS_32KCLK_DIS))
        return false;

    //If adjustable parameters updated, store in NVS
	if (state->adj_updated)
		audioRAMToNVSLoadAdjConfig(state);

#endif
    //Stop I2S stream (clock source)
	i2s_stopStream(&state->peri->i2s);

    //Turn off Vdsp2
    powerui_post_cmdwait(state->pwr, PWR_CMD_LSLDO_DIS, &state->sem_cmd, 500);

    //De-Initialize the CS pin
	audio_deinit_SPICS(state);

    //Update status
    state->status = CS_Sleeping;

    return true;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* SYSTEM STATUS -------------------------------------------------------*/
/*----------------------------------------------------------------------*/

CSStatus audio_status(AudioState *state)
{
    return state->status;
}


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* DSP CONTROL ---------------------------------------------------------*/
/*----------------------------------------------------------------------*/

bool audio_init_DSPs(AudioState *state)
{
    uint8_t core = 0;
    //Clear DSP Memory
    while(core < AUDIO_DSP_CORES_MAX)
    {
        if (state->dsp_config[core].enable)
        {
            if (!audio_write_address(state, AUD_DSP_CFG1_BASE + (core * AUD_DSP_ADDR_JUMP),
                                     AUD_DSP_CFG1_MEMCLEAR))
                return false;

            if (!audio_read_address(state, AUD_DSP_CFG1_BASE + (core * AUD_DSP_ADDR_JUMP),
                                    &state->spi_read))
                return false;

            if (state->spi_read != AUD_DSP_CFG1_MEMCLEAR)
                return false;

            rtos_Sleep_mS(50);
        }

        core++;
    }

    //Enable DSP Memory and set DSP clock
    core = 0;
    while(core < AUDIO_DSP_CORES_MAX)
    {
        if (state->dsp_config[core].enable)
        {
            if (!audio_write_address(state, AUD_DSP_CFG2_BASE + (core * AUD_DSP_ADDR_JUMP),
                                     state->dsp_config[core].clockreg))
                return false;
            if (!audio_read_address(state, AUD_DSP_CFG2_BASE + (core * AUD_DSP_ADDR_JUMP),
                                    &state->spi_read))
                return false;
            if (state->spi_read != state->dsp_config[core].clockreg)
                return false;

            if (!audio_write_address(state, AUD_DSP_CFG1_BASE + (core * AUD_DSP_ADDR_JUMP),
                                     AUD_DSP_CFG1_MEMEN))
                return false;
            if (!audio_read_address(state, AUD_DSP_CFG1_BASE + (core * AUD_DSP_ADDR_JUMP),
                                    &state->spi_read))
                return false;
            if (state->spi_read != AUD_DSP_CFG1_MEMEN)
                return false;

            rtos_Sleep_mS(20);
        }
        core++;
    }

    //Load every single element in DSP_PROG and DSP_PARAM chunks
    uint16_t memblk_elem = 0;
    bool dsp_loadsuccess = true;
    AudMemStatus dsp_loadstat = AUDMEM_SUCCESS;
    while(dsp_loadstat == AUDMEM_SUCCESS)
    {
        dsp_loadstat = audioNVSToDSPLoadAudMemBlock(state, &state->peri->mem.chunks[AUD_MEMCHUNK_DSPPROG_IND], memblk_elem);
        memblk_elem++;
        if (dsp_loadstat != AUDMEM_SUCCESS && dsp_loadstat != AUDMEM_NOELEM)
            dsp_loadsuccess = false;
    }

    dsp_loadstat = AUDMEM_SUCCESS;
    memblk_elem = 0;
    while(dsp_loadstat == AUDMEM_SUCCESS)
    {
        dsp_loadstat = audioNVSToDSPLoadAudMemBlock(state, &state->peri->mem.chunks[AUD_MEMCHUNK_DSPPARAM_IND], memblk_elem);
        memblk_elem++;
        if (dsp_loadstat != AUDMEM_SUCCESS && dsp_loadstat != AUDMEM_NOELEM)
            dsp_loadsuccess = false;
    }

    return dsp_loadsuccess;
}

/*----------------------------------------------------------------------*/

bool audio_start_DSP(AudioState *state, uint32_t core)
{
    if (!state->dsp_config[core].enable)
        return true;

    if (!audio_write_address(state, AUD_DSP_CFG1_BASE + (core * AUD_DSP_ADDR_JUMP), AUD_DSP_CFG1_COREEN))
        return false;
    if (!audio_read_address(state, AUD_DSP_CFG1_BASE + (core * AUD_DSP_ADDR_JUMP), &state->spi_read))
        return false;
    if (state->spi_read != AUD_DSP_CFG1_COREEN)
        return false;

    if (!audio_read_address(state, AUD_DSP_STAT3_BASE + (core * AUD_DSP_ADDR_JUMP),
                            &state->spi_read))
        return false;
    if (state->spi_read != state->dsp_config[core].clockreg)
        return false;

    rtos_Sleep_mS(50);

    if (!audio_write_address(state, AUD_DSP_CFG1_BASE + (core * AUD_DSP_ADDR_JUMP), AUD_DSP_CFG1_START))
        return false;

    rtos_Sleep_mS(100);

    return true;
}

/*----------------------------------------------------------------------*/

bool audio_stop_DSP(AudioState *state, uint32_t core)
{
    /*if (!audio_write_address(state, AUD_DSP_CFG1_BASE + (core * AUD_DSP_ADDR_JUMP), AUD_DSP_CFG1_COREEN))
        return false;
    if (!audio_read_address(state, AUD_DSP_CFG1_BASE + (core * AUD_DSP_ADDR_JUMP), &state->spi_read))
        return false;*/
    /*if (state->spi_read != AUD_DSP_CFG1_COREEN)
        return false;*/

    if (!audio_write_address(state, AUD_DSP_CFG1_BASE + (core * AUD_DSP_ADDR_JUMP), AUD_DSP_CFG1_MEMEN))
        return false;
    if (!audio_read_address(state, AUD_DSP_CFG1_BASE + (core * AUD_DSP_ADDR_JUMP), &state->spi_read))
        return false;
    /*if (state->spi_read != AUD_DSP_CFG1_MEMEN)
        return false;*/

    return true;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VOLUME CONTROL ------------------------------------------------------*/
/*----------------------------------------------------------------------*/

bool audio_Input_SetAll(AudioState *state)
{
    bool update = false;

    //Set up gains/volume
    for (uint16_t j = 0; j < AUD_INPUT_NUM_CHANNELS; j++)
    {
        if (j == AUD_INPUT_NUM_CHANNELS-1)
            update = true;
        else
            update = false;

        if (!audioRAMToDSPLoadInputConfig(state, j, update))
            return false;
    }

    rtos_Sleep_mS(50);

    return true;
}

/*----------------------------------------------------------------------*/

bool audio_Input_VolumeSet(AudioState *state, uint8_t channel, int16_t volume)
{
    if (channel > AUD_INPUT_NUM_CHANNELS)
        return false;

    int16_t vol;

    if (volume > AUD_INVOL_MAXDB)
        vol = AUD_INVOL_MAXDB;
    if (volume < AUD_INVOL_MINDB)
        vol = AUD_INVOL_MINDB;
    else
        vol = volume;
    state->input_config[channel].volume = vol;

    return audioRAMToDSPLoadInputConfig(state, channel, true);
}

/*----------------------------------------------------------------------*/

bool audio_Input_MuteSet(AudioState *state, uint8_t channel, bool mute)
{
    if (channel > AUD_INPUT_NUM_CHANNELS)
        return false;

    state->input_config[channel].mute = mute;

    return audioRAMToDSPLoadInputConfig(state, channel, true);
}

/*----------------------------------------------------------------------*/

bool audio_Output_SetAll(AudioState *state)
{
    bool update = false;

    //Set up gains/volume
    for (uint16_t j = 0; j < AUD_OUTPUT_NUM_CHANNELS; j++)
    {
        if (j == AUD_OUTPUT_NUM_CHANNELS-1)
            update = true;
        else
            update = false;

        if (!audioRAMToDSPLoadOutputConfig(state, j, update))
            return false;
    }

    rtos_Sleep_mS(150);

    return true;
}

/*----------------------------------------------------------------------*/

bool audio_Output_VolumeSet(AudioState *state, uint8_t channel, int16_t volume)
{
    if (channel > AUD_OUTPUT_NUM_CHANNELS)
        return false;

    int16_t vol;

    if (volume > AUD_OUTVOL_MAXDB)
        vol = AUD_OUTVOL_MAXDB;
    else if (volume < AUD_OUTVOL_MINDB)
        vol = AUD_OUTVOL_MINDB;
    else
        vol = volume;
    state->output_config[channel].volume = vol;

    return audioRAMToDSPLoadOutputConfig(state, channel, true);
}

/*----------------------------------------------------------------------*/

bool audio_Output_MuteSet(AudioState *state, uint8_t channel, bool mute)
{
    if (channel > AUD_OUTPUT_NUM_CHANNELS)
        return false;

    state->output_config[channel].mute = mute;

    return audioRAMToDSPLoadOutputConfig(state, channel, true);
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* EQ CONTROL ----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

bool audio_EQ_Set(AudioState *state, uint8_t channel, int16_t *bands)
{
	memcpy(state->eq_config[channel].band, bands, sizeof(int16_t)*10);

	return false;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* PLAY AUDIO CLIP -----------------------------------------------------*/
/*----------------------------------------------------------------------*/

//Play a file pointed to by audfile_chunk and audfile_index.
//Non blocking, but returns when file done/aborted.
void audio_playfile(AudioState *state, uint16_t audfile_index, SemaphoreHandle_t *sem)
{
	//Enable required AIF channel
	audio_AIF_enable(state, AUDIO_AIF_CH_UC, true);

	//Play file
	//TODO: Fill
	//Refer to System_State_v0.7

	//Disable AIF channel
	audio_AIF_enable(state, AUDIO_AIF_CH_UC, false);
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* AIF CONTROL ---------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void audio_AIF_enable(AudioState *state, uint8_t channel, bool enable)
{
	if (enable)
	{
		audio_write_address(state, (AUDIO_AIF_RX_START + (AUDIO_AIF_JUMP * channel)),
				AUDIO_AIF_ENABLE);
		audio_write_address(state, (AUDIO_AIF_TX_START + (AUDIO_AIF_JUMP * channel)),
				AUDIO_AIF_DISABLE);
	}
	else
	{
		audio_write_address(state, (AUDIO_AIF_RX_START + (AUDIO_AIF_JUMP * channel)),
				AUDIO_AIF_ENABLE);
		audio_write_address(state, (AUDIO_AIF_TX_START + (AUDIO_AIF_JUMP * channel)),
				AUDIO_AIF_DISABLE);
	}
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* SPI CONTROL ---------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void audio_init_SPICS(AudioState *state)
{
#ifndef AUD_EXTSPI
	HAL_GPIO_WritePin(state->cs_port, state->cs_pin, GPIO_PIN_SET);
#endif
}

/*----------------------------------------------------------------------*/

void audio_deinit_SPICS(AudioState *state)
{
#ifndef AUD_EXTSPI
	HAL_GPIO_WritePin(state->cs_port, state->cs_pin, GPIO_PIN_RESET);
#endif
}
