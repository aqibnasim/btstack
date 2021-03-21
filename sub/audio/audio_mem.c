#include "audio_mem.h"

/* LOCAL FUNCTIONS -----------------------------------------------------*/

/*! \brief Fill SPI buffer with header

    \param *buff Pointer to buffer
    \param addr Address on CS47L90
*/
static void audio_fill_bufferheader(uint16_t *buff, uint32_t addr);

/*! \brief Fill SPI buffer with address and data

    \param *buff Pointer to buffer
    \param addr Address on CS47L90
    \param data Data to write
    \param data_long Is the data 32bit?
*/
static void audio_fill_buffer(uint16_t *buff, uint32_t addr, uint32_t data, bool data_long);

/*! \brief Enter a read bit into Transmit buffer

    \param *spi Pointer to SPI Object
    \param *txbuff Pointer to transmit buffer
*/
static void audio_fill_readbit(SpiObject *spi, uint16_t *txbuff);

/*! \brief Read data from the receive SPI buffer

    \param *buff Pointer to receive buffer
    \param data_long Is the data 32-bit?
*/
static uint32_t audio_read_buffer(uint16_t *rxbuff, bool data_long);

/*! \brief Process an SPI transfer

    \param *state Pointer to Audio Object
    \param word_count Number of 16-bit words to read/write
    \param read Is this a READ?
*/
static bool audio_spi_transfer(AudioState *state, size_t word_count, bool read);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* 1) NVS MEMORY to CODEC/DSP ------------------------------------------*/
/*----------------------------------------------------------------------*/

bool audioNVSToDSPLoadAudMemReg(AudioState *state, MemChunk *chunk, uint16_t elem_index)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    MemElement elem;
    size_t elem_offset;

    //Extract Element details
    if (memReadElementParam(&state->peri->mem, chunk, &elem, elem_index) != MEM_OP_SUCCESS)
        return false;

    //Read header
    uint16_t num_regs;
    elem_offset = 0;
    if (memReadFromElement(&state->peri->mem, chunk, &elem, elem_offset, (void *)&num_regs, sizeof(num_regs))
            != MEM_OP_SUCCESS)
        return false;

    elem_offset += sizeof(num_regs);

    //Start loading each AudMemReg in element to DSP
    AudMemReg data;
    uint16_t index = 0;
#ifdef AUDIO_DEBUG
    uint32_t tmp;
#endif
    while(index < num_regs)
    {
        if (memReadFromElement(&state->peri->mem, chunk, &elem, elem_offset, (void *)&data, sizeof(data))
            != MEM_OP_SUCCESS)
            return false;

        if (!audio_write_address(state, data.offset, data.data))
            return false;

#ifdef AUDIO_DEBUG
        if (!audio_read_address(state, data.offset, &tmp))
			return false;
#endif
        index++;
        elem_offset += sizeof(data);
    }

    return true;
}

/*----------------------------------------------------------------------*/

AudMemStatus audioNVSToDSPLoadAudMemBlock(AudioState *state, MemChunk *chunk, uint16_t elem_index)
{
#ifdef AUD_EXTSPI
	return AUDMEM_SUCCESS;
#endif
    MemElement elem;
    size_t elem_offset;

    //Extract Element details
    if (memReadElementParam(&state->peri->mem, chunk, &elem, elem_index) != MEM_OP_SUCCESS)
        return AUDMEM_NOELEM;

    //Read header
    AudMemBlock memblk;
    elem_offset = 0;
    if (memReadFromElement(&state->peri->mem, chunk, &elem, elem_offset, (void *)&memblk, sizeof(memblk))
            != MEM_OP_SUCCESS)
        return AUDMEM_MEMFAIL;

    elem_offset += sizeof(memblk);

    //Determine whether our destination registers are 32bit
    if (memblk.offset < AUDIO_DSP_32BIT_START)
        return AUDMEM_BADDATA;

    //Check if data length is correct (if it "fits")
    if ((memblk.length % sizeof(uint32_t)) != 0)
        return AUDMEM_BADDATA;

    //Determine how many transfers we'll need to make based on size of buffer and length of block
    uint32_t transfer_count;
    uint32_t leftover_count;

    if (memblk.length < CS_SPI_WBYTEMAX)
    {
        transfer_count = 1;
        leftover_count = memblk.length;
    }
    else
    {
        transfer_count = memblk.length / AUDIO_SPIBUFF_BYTESZ;
        leftover_count = memblk.length % AUDIO_SPIBUFF_BYTESZ;

        if (leftover_count != 0)
            transfer_count++;
    }

    size_t start_address, word_count, data_size;

    //Start making the transfers
    for (uint16_t i = 0; i < transfer_count; i++)
    {
        if (i == (transfer_count - 1) && leftover_count != 0)
        {
            data_size = leftover_count;
        }
        else
            data_size = AUDIO_SPIBUFF_BYTESZ;

        start_address = (memblk.offset + (i * (AUDIO_SPIBUFF_BYTESZ >> 1)));
        word_count = (data_size >> 1) + CS_SPI_HEADERSZ;

        //Read from memory to our TX Buffer
        if (memReadFromElement(&state->peri->mem, chunk, &elem,
                               elem_offset + (i * AUDIO_SPIBUFF_BYTESZ),
                               (void*)(state->spi_trans.write_buff + AUDIO_SPIBUFF_HEADERBYTESZ),
                               data_size)
                != MEM_OP_SUCCESS)
            return AUDMEM_MEMFAIL;

        //Populate the header
        audio_fill_bufferheader((uint16_t*)state->spi_trans.write_buff,
                                start_address);

        //Transfer to CODEC/DSP
        if (!audio_spi_transfer(state, word_count, false))
            return AUDMEM_SPIW_FAIL;

        //Read back
#ifdef AUD_LOAD_READBACK
        uint32_t readaddr = 0;

        uint32_t readback;
        uint32_t readbackmem, readbackmemH, readbackmemL;

        for (uint32_t rbpos = 0; rbpos < (data_size >> 1); rbpos+=2)
        {
            readback = audio_read_buffer(state->spi_buff.audio_write_buff + rbpos, true);

            readaddr = start_address + rbpos;
            audio_read_address(state, readaddr, &state->spi_read);
            memReadFromElement(&state->peri->mem, chunk, &elem,
                               elem_offset + (i * AUDIO_SPIBUFF_BYTESZ) + (rbpos*2),
                               &readbackmemH,
                               sizeof(uint16_t));

            memReadFromElement(&state->peri->mem, chunk, &elem,
                               elem_offset + (i * AUDIO_SPIBUFF_BYTESZ) + (rbpos*2) + 2,
                               &readbackmemL,
                               sizeof(uint16_t));

            readbackmem = (readbackmemH << 16) + readbackmemL;

            if (state->spi_read != readback)
            {
                state->debug1++;
            }
        }
#endif
        rtos_Sleep_mS(10);
    }

    return AUDMEM_SUCCESS;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* 2) RAM to CODEC/DSP -------------------------------------------------*/
/*----------------------------------------------------------------------*/

bool audioRAMToDSPLoadInputConfig(AudioState *state, uint8_t channel, bool update)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    //Load adjustment
    int16_t volume = ((state->input_config[channel].volume + state->input_adj[channel] + abs(AUD_INVOL_MINDB))
            / AUD_INVOL_DBMUL);

    //Unmute/mute and update volume at same time
    if (state->input_config[channel].mute)
        volume |= (1 << 8);

    if (update)
        volume |= (1 << 9);

    if (!audio_write_address(state,
           (AUD_INPUT_VOL_REGBASE + (channel * AUD_INPUT_VOL_REGHOP)), volume))
        return false;

    return true;
}

/*----------------------------------------------------------------------*/

bool audioRAMToDSPLoadOutputConfig(AudioState *state, uint8_t channel, bool update)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    //Load adjustment
    int16_t volume = ((state->output_config[channel].volume + state->output_adj[channel] + abs(AUD_OUTVOL_MINDB))
            / AUD_OUTVOL_DBMUL);

    //Unmute/mute and update volume at same time
    if (state->output_config[channel].mute)
        volume |= (1 << 8);

    if (update)
        volume |= (1 << 9);

    if (!audio_write_address(state,
           (AUD_OUTPUT_VOL_REGBASE + (channel * AUD_OUTPUT_VOL_REGHOP)), volume))
        return false;

    return true;
}

/*----------------------------------------------------------------------*/

bool audioRAMToDSPLoadEQConfig(AudioState *state, uint8_t channel, bool update)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    //TODO: populate

    return true;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* 3) RAM to/from NVS MEMORY -------------------------------------------*/
/*----------------------------------------------------------------------*/

bool audioRAMToNVSLoadAdjConfig(AudioState *state)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    //Erase AUD_ADJ Chunk
    if (memInitChunk(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND]) != MEM_OP_SUCCESS)
        return false;

    MemElement elem;
    uint16_t elemindex;

    //Add INPUT
    if (memAddElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND], (sizeof(AudioInputConfig) * AUD_INPUT_NUM_CHANNELS), &elem, &elemindex)
            != MEM_OP_SUCCESS)
        return false;

    if (memWriteToElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND], &elem, 0,
            (void *)state->input_config,
            (sizeof(AudioInputConfig) * AUD_INPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
      return false;

    //Add OUTPUT
    if (memAddElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND], (sizeof(AudioOutputConfig) * AUD_OUTPUT_NUM_CHANNELS), &elem, &elemindex)
            != MEM_OP_SUCCESS)
        return false;

    if (memWriteToElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND], &elem, 0,
             (void *)state->output_config,
             (sizeof(AudioOutputConfig) * AUD_OUTPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
       return false;

    //Add INPUT adjustment
    if (memAddElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND], (sizeof(int16_t) * AUD_INPUT_NUM_CHANNELS), &elem, &elemindex)
            != MEM_OP_SUCCESS)
       return false;

    if (memWriteToElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND], &elem, 0,
           (void *)state->input_adj,
           (sizeof(int16_t) * AUD_INPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
     return false;

    //Add OUPTUT adjustment
    if (memAddElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND], (sizeof(int16_t) * AUD_OUTPUT_NUM_CHANNELS), &elem, &elemindex)
           != MEM_OP_SUCCESS)
       return false;

    if (memWriteToElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND], &elem, 0,
           (void *)state->output_adj,
           (sizeof(int16_t) * AUD_OUTPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
     return false;

    //Add EQ Configuration
	if (memAddElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND], (sizeof(AudioEQConfig) * AUD_EQ_NUM_CHANNELS), &elem, &elemindex)
			!= MEM_OP_SUCCESS)
		return false;

	if (memWriteToElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND], &elem, 0,
			 (void *)state->eq_config,
			 (sizeof(AudioEQConfig) * AUD_EQ_NUM_CHANNELS)) != MEM_OP_SUCCESS)
	   return false;

    return true;
}

/*----------------------------------------------------------------------*/

bool audioRAMToNVSLoadDSPConfig(AudioState *state)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    //Erase AUD_DSPCFG Chunk
    if (memInitChunk(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_DSPCFG_IND]) != MEM_OP_SUCCESS)
        return false;

    MemElement elem;
    uint16_t elemindex;

    //Add DSP
    if (memAddElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_DSPCFG_IND], (sizeof(AudioDSPConfig) * AUDIO_DSP_CORES_MAX), &elem, &elemindex)
            != MEM_OP_SUCCESS)
        return false;

    if (memWriteToElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_DSPCFG_IND], &elem, 0,
            (void *)state->dsp_config,
            (sizeof(AudioDSPConfig) * AUDIO_DSP_CORES_MAX)) != MEM_OP_SUCCESS)
      return false;

    return true;
}

/*----------------------------------------------------------------------*/

bool audioNVSToRAMLoadDSPConfig(AudioState *state)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    MemElement elem;

    //Extract Element details
    if (memReadElementParam(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_DSPCFG_IND],
            &elem, AUD_DSPCFG_MEMELEM_INDEX) != MEM_OP_SUCCESS)
       return false;

    //Extract element
    if (memReadFromElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_DSPCFG_IND],
           &elem, 0, (void *)state->dsp_config,
           (sizeof(AudioDSPConfig) * AUDIO_DSP_CORES_MAX)) != MEM_OP_SUCCESS)
        return false;

    return true;
}

/*----------------------------------------------------------------------*/

bool audioNVSToRAMLoadOutputConfig(AudioState *state)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    MemElement elem;

    //Extract Element details
    if (memReadElementParam(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND],
            &elem, AUD_OUTPUTVOL_MEMELEM_INDEX) != MEM_OP_SUCCESS)
       return false;

    //Extract element
    if (memReadFromElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND],
           &elem, 0, (void *)state->output_config,
           (sizeof(AudioOutputConfig) * AUD_OUTPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
        return false;

    //Extract Element details
    if (memReadElementParam(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND],
            &elem, AUD_OUTPUTADJ_MEMELEM_INDEX) != MEM_OP_SUCCESS)
       return false;

    //Extract element
    if (memReadFromElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND],
           &elem, 0, (void *)state->output_adj,
           (sizeof(int16_t) * AUD_OUTPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
        return false;
    return true;
}

/*----------------------------------------------------------------------*/

bool audioNVSToRAMLoadInputConfig(AudioState *state)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    MemElement elem;

    //Extract Element details
    if (memReadElementParam(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND],
            &elem, AUD_INPUTVOL_MEMELEM_INDEX) != MEM_OP_SUCCESS)
       return false;

    //Extract element
    if (memReadFromElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND],
           &elem, 0, (void *)state->input_config,
           (sizeof(AudioInputConfig) * AUD_INPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
        return false;

    //Extract Element details
    if (memReadElementParam(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND],
            &elem, AUD_INPUTADJ_MEMELEM_INDEX) != MEM_OP_SUCCESS)
       return false;

    //Extract element
    if (memReadFromElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND],
           &elem, 0, (void *)state->input_adj,
           (sizeof(int16_t) * AUD_INPUT_NUM_CHANNELS)) != MEM_OP_SUCCESS)
        return false;

    return true;
}

/*----------------------------------------------------------------------*/

bool audioNVSToRAMLoadEQConfig(AudioState *state)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    MemElement elem;

    //Extract Element details
    if (memReadElementParam(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND],
            &elem, AUD_EQ_MEMELEM_INDEX) != MEM_OP_SUCCESS)
       return false;

    //Extract element
    if (memReadFromElement(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND],
           &elem, 0, (void *)state->eq_config,
           (sizeof(AudioEQConfig) * AUD_EQ_NUM_CHANNELS)) != MEM_OP_SUCCESS)
        return false;

    return true;
}

/*----------------------------------------------------------------------*/

bool audioRAMToNVSLoadAudMemReg(AudioState *state, MemChunk *chunk,
                                AudMemReg *reg, uint16_t num_regs)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    MemElement temp_elem;
    uint16_t elemindex;

    if (memAddElement(&state->peri->mem, chunk, (num_regs * sizeof(AudMemReg) + AUDMEMREG_HEADER_SZ), &temp_elem, &elemindex)
            != MEM_OP_SUCCESS)
        return false;

    //Write AudMemReg Header first
    if (memWriteToElement(&state->peri->mem, chunk, &temp_elem,
                          0, &num_regs, sizeof(num_regs))
            != MEM_OP_SUCCESS)
        return false;

    //Write data proper
    if (memWriteToElement(&state->peri->mem, chunk, &temp_elem,
                          sizeof(num_regs), reg, (sizeof(AudMemReg) * num_regs))
            != MEM_OP_SUCCESS)
        return false;

    return true;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* 4) Single Register Reads/Writes -------------------------------------*/
/*----------------------------------------------------------------------*/

bool audio_write_address(AudioState *state, uint32_t addr, uint32_t data)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    uint8_t word_count = 4;
    bool long_enable = false;
    if (addr >= AUDIO_DSP_32BIT_START)
    {
        word_count++;
        long_enable = true;
    }

    audio_fill_buffer((uint16_t *)state->spi_trans.write_buff, addr, data, long_enable);

    return audio_spi_transfer(state, word_count, false);
}

/*----------------------------------------------------------------------*/

bool audio_read_address(AudioState *state, uint32_t addr, uint32_t *data)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    uint8_t word_count = 4;
    bool long_enable = false;
    if (addr >= AUDIO_DSP_32BIT_START)
    {
        word_count++;
        long_enable = true;
    }

    audio_fill_buffer((uint16_t *)state->spi_trans.write_buff, addr, 0, long_enable);

    if (!audio_spi_transfer(state, word_count, true))
        return false;

    uint16_t *rx = (uint16_t *)state->spi_trans.read_buff;
    *data = rx[3];

    if (long_enable)
    {
        *data = *data << 16;
        *data += rx[4];
    }

    return true;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* UTILITIES -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/


void audio_fill_bufferheader(uint16_t *txbuff, uint32_t addr)
{
    txbuff[0] = (uint16_t)((0xFFFF0000 & addr) >> 16);
    txbuff[1] = (uint16_t)(0x0000FFFF & addr);
    txbuff[2] = 0xFFFF;
}

/*----------------------------------------------------------------------*/

void audio_fill_buffer(uint16_t *txbuff, uint32_t addr, uint32_t data, bool data_long)
{
    audio_fill_bufferheader(txbuff, addr);

    if (data_long)
    {
        txbuff[3] = (uint16_t)((0xFFFF0000 & data) >> 16);
        txbuff[4] = (uint16_t)(0x0000FFFF & data);
    }
    else
    {
        txbuff[3] = (uint16_t)(0x0000FFFF & data);
    }
}

/*----------------------------------------------------------------------*/

void audio_fill_readbit(SpiObject *spi, uint16_t *txbuff)
{
	uint8_t *tx = txbuff;
	tx++;
	*tx |= (1 << 7);
}

/*----------------------------------------------------------------------*/

uint32_t audio_read_buffer(uint16_t *rxbuff, bool data_long)
{
    uint32_t tmp = 0;

    if (data_long)
        tmp = (rxbuff[4] + ((uint32_t)rxbuff[3] << 16));
    else
        tmp = rxbuff[3];

    return tmp;
}

/*----------------------------------------------------------------------*/

bool audio_spi_transfer(AudioState *state, size_t word_count, bool read)
{
#ifdef AUD_EXTSPI
	return true;
#endif
    uint16_t retry = 0;
    SpiTransferMode mode = SPI_WRITE;

    bool success = true;

    HAL_GPIO_WritePin(state->cs_port, state->cs_pin, GPIO_PIN_RESET);

    if (read)
    {
    	audio_fill_readbit(&state->peri->spi, (uint16_t *)state->spi_trans.write_buff);
    	mode = SPI_RW;
    }

	if (!spiNewTransferWait(&state->peri->spi, &state->spi_trans, word_count, mode))
		success = false;

    HAL_GPIO_WritePin(state->cs_port, state->cs_pin, GPIO_PIN_SET);

    return success;
}





/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* DEPRECATED ----------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*



bool audioLoadAudioRegSectorFromMem(AudioState *state, MemSectorObject *sector)
{
    //Check if sector has been preconfigured
    if (!memChkSectorReady(state->mem, sector))
        return false;

    //Load each memory block to DSP
    AudioReg tmp;
    MemSectorElement block;
    size_t bite_read;

    uint16_t i;
    for (uint16_t j = 0; j < sector->num_elements; j++)
    {
        i = 0;

        if (!memReadSectorElementParam(state->mem, sector, 0, &block))
            return false;

        while (memReadSectorElementBite(state->mem, sector, &block, i,
                                        sizeof(tmp), (void *)&tmp, &bite_read))
        {
            if (tmp.addr >= AUDIO_DSP_32BIT_START)
            {
                if (!audio_write_address_32(state, tmp.addr, tmp.data))
                    return false;
            }
            else
            {
                if (!audio_write_address_16(state, tmp.addr, tmp.data))
                    return false;
            }

            i++;
        }
        rtos_Sleep_mS(10);
    }

    return true;
}

bool audioLoadDSPRegsFromMem(AudioState *state, MemSectorObject *dsp_sector)
{
    //Check if config sector has been preconfigured
    MemSectorObject *ptr_sector = dsp_sector + 1;
    if (!memChkSectorReady(state->mem, dsp_sector))
        return false;
    for (uint8_t i = 0; i < MEMSECTOR_NUM_DSPPTRSECTORS; i++)
    {
        if (!memChkSectorReady(state->mem, ptr_sector + i))
            return false;
    }

    //Clear DSP memory
    if (!audio_write_address_32(state, AUDIO_DSP_CONFIG1_ADDR, AUDIO_DSP_CONFIG1_MEMCLEAR))
        return false;

    rtos_Sleep_mS(200);

    //Load contents of config sector into AUD_IC memory
    if (!audioLoadAudioRegSectorFromMem(state, &state->mem_dsp1_sectors[MEMSECTOR_IND_AUDDSPCFG_OFFSET]))
        return false;

    //Each element in a pointer sector maps a MEM-sector to the AUD-IC memory space
    //For each element run the following
    MemSectorObject *tmp_ptr_sector = ptr_sector;
    uint16_t block_index_last = 0;
    for (uint8_t i = 0; i < MEMSECTOR_NUM_DSPPTRSECTORS; i++)
    {
        for (uint8_t j = 0; j < tmp_ptr_sector->num_elements; j++)
        {
            audioLoadAudioRegBlockSectorFromMem(state, tmp_ptr_sector, (j + block_index_last));
        }

        block_index_last += tmp_ptr_sector->num_elements;
        tmp_ptr_sector++;
    }

    return true;
}
*/

/*while (!success)
{
    //Transfer to CODEC/DSP
    if (!audio_spi_transfer(state, (data_size >> 1) + CS_SPI_HEADERSZ, false))
        return AUDMEM_SPIW_FAIL;

    //Readback
    if (!audio_spi_transfer(state, (data_size >> 1) + CS_SPI_HEADERSZ, true))
        return AUDMEM_SPIR_FAIL;

    for (uint16_t j = CS_SPI_HEADERSZ; j < (data_size >> 1) + CS_SPI_HEADERSZ; j++)
    {
       if (audio_spibuff.audio_read_buff[j] != audio_spibuff.audio_write_buff[j])
        {
            state->debug1++;
        }
    }

    success = true;
}*/

