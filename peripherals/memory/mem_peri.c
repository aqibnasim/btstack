/*! \file "mem_peri.c"
   \brief Memory Abstraction layer.
   Provides functions to interface with HAL drivers.
   Implements a simple file system consisting of "Chunks" and "Elements".
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "mem_peri.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

//Sector utility
/*! \brief Return the NVS address of the end of a given chunk

    \param *mem Pointer to MemObject object
    \param *chunk Specified chunk
    \return Chunk end
*/
static size_t memReturnChunkEnd(MemObject *mem, MemChunk *chunk);

/*! \brief Extract Element Offset (within Chunk) from element header

    \param temp Element header
    \return Offset
*/
static size_t memExtractElementOffset(uint32_t temp);

//NVS Chip Operations
/*! \brief Verify that written data is correct

    \param *mem Pointer to MemObject object
    \param offset Offset in NVS where data was written
    \param *source Source of written data in RAM
    \param size Size of data to verify
    \return Pass or Fail?
*/
static bool nvsVerify(MemObject *mem, size_t offset, void *source, size_t size);

/*! \brief Enable Write operation

    \param *mem Pointer to MemObject object
    \return Pass or Fail?
*/
static bool nvsWriteEnable(MemObject *mem);

/*! \brief Disable Write operation

    \param *mem Pointer to MemObject object
    \return Pass or Fail?
*/
static bool nvsWriteDisable(MemObject *mem);

/*! \brief Read NVS IC status

    \param *mem Pointer to MemObject object
    \param *status Destination of status information
    \return Pass or Fail?
*/
static bool nvsReadStatus(MemObject *mem, uint8_t *status);

/*! \brief Read JID of NVS

    \param *mem Pointer to MemObject object
    \return Pass or Fail?
*/
static uint32_t nvsReadJID(MemObject *mem);

/*! \brief Read NVS ID

    \param *mem Pointer to MemObject object
    \return Pass or Fail?
*/
static uint8_t nvsReadID(MemObject *mem);

/*! \brief Reset NVS IC

    \param *mem Pointer to MemObject object
    \return Pass or Fail?
*/
static bool nvsReset(MemObject *mem);

/*! \brief Erase entire NVS IC

    \param *mem Pointer to MemObject object
    \return Pass or Fail?
*/
static bool nvsChipErase(MemObject *mem);

/*! \brief Wait till NVS ready

    \param *mem Pointer to MemObject object
    \return Ready?
*/
static bool nvsWaitReady(MemObject *mem);

/*! \brief Enter Deep powerdown

    \param *mem Pointer to MemObject
    \return Pass or Fail?
*/
static bool nvsEnterDeepPowerdown(MemObject *mem);

/*! \brief Exit Deep powerdown

    \param *mem Pointer to MemObject
    \return Pass or Fail?
*/
static bool nvsExitDeepPowerdown(MemObject *mem);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

bool memInit(MemObject *mem, SPI_HandleTypeDef *handle, uint8_t spi_index,
		GPIO_TypeDef *cs_port, uint16_t cs_pin,
		MemChunk *chunks, uint16_t num_chunks, size_t sectorSize, size_t totalSize)
{
	spiInit(&mem->spi, handle, spi_index);

	mem->cs_port = cs_port;
	mem->cs_pin = cs_pin;

    rtos_Sleep_mS(100);
	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);
    rtos_Sleep_mS(100);

    bool result = true;

    //Wake up from sleep
    nvsExitDeepPowerdown(mem);

	mem->sectorSize = sectorSize;
	mem->totalSize = totalSize;
    mem->num_chunks = num_chunks;
    mem->chunks = chunks;

    mem->chip_id = nvsReadJID(mem);
    mem->dev_id = nvsReadID(mem);

    //Reset
    nvsReset(mem);

    for (uint16_t i = 0; i < num_chunks; i++)
    {
        if (memCheckChunk(mem, &mem->chunks[i]) != MEM_OP_SUCCESS)
            if (memInitChunk(mem, &mem->chunks[i]) != MEM_OP_SUCCESS)
                result = false;
    }

    //Set to deep sleep
    nvsEnterDeepPowerdown(mem);

    return result;
}


/*----------------------------------------------------------------------*/

bool memCheckBusy(MemObject *mem)
{
	return spiCheckBusy(&mem->spi);
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CHUNK LEVEL FUNCTIONS -----------------------------------------------*/
/*----------------------------------------------------------------------*/

MemOpStatus memInitChunk(MemObject *mem, MemChunk *chunk)
{
	MemOpStatus result = MEM_OP_SUCCESS;

	//Wake up from sleep
	nvsExitDeepPowerdown(mem);

    //Check parameters
    if ((chunk->num_sectors > MEM_CHUNK_MAXSECTORS) && (chunk->num_sectors <= MEM_CHUNK_MINSECTORS))
    {
    	result = MEM_OP_FAIL;
		goto END;
	}

    if ((chunk->offset % mem->sectorSize != 0) &&
       (chunk->offset < MEM_START_ADDRESS) &&
       ((memReturnChunkEnd(mem, chunk) - 1) > MEM_END_ADDRESS))
    {
    	result = MEM_OP_FAIL;
    	goto END;
    }

    if (!nvsErase(mem, chunk->offset, chunk->num_sectors))
    {
    	result = MEM_NVS_FAIL;
    	goto END;
    }

    if (!nvsWriteFrom(mem, chunk->offset, &chunk->num_sectors, sizeof(chunk->num_sectors)))
    {
    	result = MEM_NVS_FAIL;
    	goto END;
    }

    //Set to deep sleep
END:
	nvsEnterDeepPowerdown(mem);

    return result;
}

/*----------------------------------------------------------------------*/

MemOpStatus memCheckChunk(MemObject *mem, MemChunk *chunk)
{
    uint16_t temp;
    MemOpStatus result = MEM_OP_SUCCESS;

	//Wake up from sleep
	nvsExitDeepPowerdown(mem);

    if (!nvsReadTo(mem, chunk->offset, &temp, MEM_CHUNKBLK_SIZE))
    {
    	result =  MEM_NVS_FAIL;
    	goto END;
    }

    //Chunk written
    if ((temp == MEM_CHUNK_EMPTY))
    {
    	result =  MEM_OP_FAIL;
    	goto END;
    }

    //Check if details match
    if (temp != chunk->num_sectors)
    {
    	result =  MEM_OP_FAIL;
    	goto END;
    }
    //Set to deep sleep
END:
	nvsEnterDeepPowerdown(mem);

	return result;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* ELEMENT LEVEL FUNCTIONS ---------------------------------------------*/
/*----------------------------------------------------------------------*/

MemOpStatus memAddElement(MemObject *mem, MemChunk *chunk, size_t elem_size, MemElement *elem_return, uint16_t *elemindex_return)
{
    //Find last element
    uint16_t elem_index = 0;
    MemElement temp;
    temp.offset = MEM_CHUNKBLK_SIZE;
    temp.size = 0;
    MemOpStatus result;

    while(1)
    {
        result = memReadElementParam(mem, chunk, &temp, elem_index);

        if (result == MEM_NVS_FAIL)
            return result;
        else if (result == MEM_OP_FAIL)
            break;
        else
            elem_index++;
    }


    result = MEM_OP_SUCCESS;

	//Wake up from sleep
	nvsExitDeepPowerdown(mem);

    //Check if new element will overflow
    size_t new_elem_end = temp.offset + temp.size + elem_size;
    size_t elemblock_offset = memReturnChunkEnd(mem, chunk) - ((elem_index + 1) * MEM_ELEMBLK_SIZE);
    if ((new_elem_end + chunk->offset) > elemblock_offset)
    {
    	result = MEM_OP_FAIL;
    	goto END;
	}

    uint32_t temp_elem = (new_elem_end << MEM_ELEMBLK_OFFSET_SHIFT) & ~MEM_ELEMBLK_WRITTEN_MASK;

    //Write a new Element-Block
    if (!nvsWriteFrom(mem, elemblock_offset, &temp_elem, MEM_ELEMBLK_SIZE))
    {
    	result = MEM_NVS_FAIL;
    	goto END;
    }

    //Return element
    elem_return->offset = temp.offset + temp.size;
    elem_return->size = elem_size;

    *elemindex_return = elem_index;


    //Set to deep sleep
END:
	nvsEnterDeepPowerdown(mem);

    return result;
}

/*----------------------------------------------------------------------*/

MemOpStatus memWriteToElement(MemObject *mem, MemChunk *chunk, MemElement *elem, size_t offset,
                       void *source, size_t write_size)
{
    MemOpStatus result = MEM_OP_SUCCESS;

	//Wake up from sleep
	nvsExitDeepPowerdown(mem);

    //Check if offset valid
    if ((offset + write_size) > elem->size)
    {
    	result = MEM_OP_FAIL;
		goto END;
	}

    if (!nvsWriteFrom(mem, (chunk->offset + elem->offset + offset), source, write_size))
    {
    	result = MEM_NVS_FAIL;
    	goto END;
    }

    //Set to deep sleep
END:
	nvsEnterDeepPowerdown(mem);

    return result;
}

/*----------------------------------------------------------------------*/

MemOpStatus memReadElementParam(MemObject *mem, MemChunk *chunk, MemElement *elem,
                    uint16_t element_index)
{

    MemOpStatus result = MEM_OP_SUCCESS;

	//Wake up from sleep
	nvsExitDeepPowerdown(mem);

    size_t offset = memReturnChunkEnd(mem, chunk) - ((element_index + 1) * MEM_ELEMBLK_SIZE);
    uint32_t temp;
    if (!nvsReadTo(mem, offset, &temp, MEM_ELEMBLK_SIZE))
    {
    	result = MEM_NVS_FAIL;
    	goto END;
    }

    if ((temp & MEM_ELEMBLK_WRITTEN_MASK) != 0)
    {
    	result = MEM_OP_FAIL;
    	goto END;
    }

    size_t end_offset = memExtractElementOffset(temp);

    //If first element-block, return value
    if (element_index == 0)
    {
        elem->size = end_offset - MEM_CHUNKBLK_SIZE;
        elem->offset = MEM_CHUNKBLK_SIZE;
        result = MEM_OP_SUCCESS;
        goto END;
    }

    //If not first, get preceding element-block
    offset = memReturnChunkEnd(mem, chunk) - (element_index * MEM_ELEMBLK_SIZE);
    if (!nvsReadTo(mem, offset, &temp, MEM_ELEMBLK_SIZE))
    {
    	result = MEM_NVS_FAIL;
    	goto END;
    }

    if ((temp & MEM_ELEMBLK_WRITTEN_MASK) != 0)
    {
    	result = MEM_OP_FAIL;
    	goto END;
    }

    elem->offset = memExtractElementOffset(temp);
    elem->size = end_offset - elem->offset;

    //Set to deep sleep
END:
	nvsEnterDeepPowerdown(mem);

    return result;
}

/*----------------------------------------------------------------------*/

MemOpStatus memReadFromElement(MemObject *mem, MemChunk *chunk, MemElement *elem, size_t offset,
                        void *dest, size_t read_size)
{
    MemOpStatus result = MEM_OP_SUCCESS;

	//Wake up from sleep
	nvsExitDeepPowerdown(mem);

    //Check if offset valid
    if ((offset + read_size) > elem->size)
    {
    	result = MEM_OP_FAIL;
    	goto END;
    }

    if (!nvsReadTo(mem, (chunk->offset + elem->offset + offset), dest, read_size))
    {
    	result = MEM_NVS_FAIL;
    	goto END;
    }


    //Set to deep sleep
END:
	nvsEnterDeepPowerdown(mem);

    return result;
}

/*----------------------------------------------------------------------*/

uint8_t memCheckHashOAD(MemObject *mem, MemChunk *chunk, uint8_t *refHash)
{
	MemElement elem;
	memReadElementParam(mem, chunk, &elem, 0);

	//Check if recorded hash is same as reference hash
	memReadFromElement(mem, chunk, &elem, OAD_IMAGE_HEADER_HASHOFFS,
			mem->buffer, OAD_IMAGE_HEADER_HASHSZ);

	for (uint8_t i = 0; i < OAD_IMAGE_HEADER_HASHSZ; i++)
	{
		if (mem->buffer[i] != refHash[i])
			return OAD_HASHVERIFY_CALCMISMATCH;
	}

	//Go through our OAD image and calculate SHA256 hash
	size_t offset = OAD_IMAGE_HEADER_SIZE;
	uint8_t calcHash[OAD_IMAGE_HEADER_HASHSZ];

	calc_sha_256(calcHash, mem, chunk, &elem, offset, elem.size - OAD_IMAGE_HEADER_SIZE);

	for (uint8_t i = 0; i < OAD_IMAGE_HEADER_HASHSZ; i++)
	{
		if (calcHash[i] != refHash[i])
			return OAD_HASHVERIFY_CALCMISMATCH;
	}

	return OAD_HASHVERIFY_SUCCESS;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* SECTOR UTILITY FUNCTIONS --------------------------------------------*/
/*----------------------------------------------------------------------*/

size_t memReturnChunkEnd(MemObject *mem, MemChunk *chunk)
{
    size_t end = chunk->offset;
    end += chunk->num_sectors * mem->sectorSize;

    return end;
}

/*----------------------------------------------------------------------*/

size_t memExtractElementOffset(uint32_t temp)
{
    return (temp & MEM_ELEMBLK_OFFSET_MASK) >> MEM_ELEMBLK_OFFSET_SHIFT;
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* NVS DRIVER FUNCTIONS ------------------------------------------------*/
/*----------------------------------------------------------------------*/

bool nvsReadTo(MemObject *mem, size_t offset, void *dest, size_t size)
{
	uint8_t txbuff[4];

	if (!nvsWaitReady(mem))
		return false;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

	spiInitTransaction(&mem->trans, NULL, txbuff);

	txbuff[0] = MEM_CMD_RDLP;
	txbuff[1] = (offset & 0xFF0000) >> 16;
	txbuff[2] = (offset & 0xFF00) >> 8;
	txbuff[3] = (offset & 0xFF);

	bool success = false;

	if (spiNewTransferWait(&mem->spi, &mem->trans, 4, SPI_WRITE))
	{
		spiInitTransaction(&mem->trans, dest, NULL);
		if (spiNewTransferWait(&mem->spi, &mem->trans, size, SPI_READ))
			success = true;
	}

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

	return success;
}

/*----------------------------------------------------------------------*/

bool nvsWriteFrom(MemObject *mem, size_t offset, void *source, size_t size)
{
	uint8_t txbuff[4];

	bool success = false;

	uint8_t *srcBuf = source;
	size_t length = size;
	size_t ilen = 0;
	size_t foffset = offset;

	while (length > 0)
	{
		//Check if Ready
		if (!nvsWaitReady(mem))
			break;

		//Enable write
		if (!nvsWriteEnable(mem))
			break;

		ilen = MEM_PP_MAXBYTES - (foffset % MEM_PP_MAXBYTES);
		if (length < ilen)
			ilen = length;

		txbuff[0] = MEM_CMD_PP;
		txbuff[1] = (foffset & 0xFF0000) >> 16;
		txbuff[2] = (foffset & 0xFF00) >> 8;
		txbuff[3] = (foffset & 0xFF);

		foffset += ilen;
		length -= ilen;

		HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

		//Write command
		spiInitTransaction(&mem->trans, NULL, txbuff);
		if (!spiNewTransferWait(&mem->spi, &mem->trans, sizeof(txbuff), SPI_WRITE))
			break;

		//Write data
		spiInitTransaction(&mem->trans, NULL, srcBuf);
		if (!spiNewTransferWait(&mem->spi, &mem->trans, ilen, SPI_WRITE))
			break;

		srcBuf += ilen;

		HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

		if (length == 0)
			success = true;
	}

	//Verify
	if (success)
		return nvsVerify(mem, offset, source, size);
	else
		return false;
}

/*----------------------------------------------------------------------*/

bool nvsErase(MemObject *mem, size_t offset, uint16_t num_sectors)
{
	uint8_t txbuff[5];

	bool success = false;

	//Erase each sector
	size_t sector_erase = 0;
	for (uint16_t sector = 0; sector < num_sectors; sector++)
	{
		//Write Enable
		if (!nvsWriteEnable(mem))
			break;

		sector_erase = offset + (sector * mem->sectorSize);

		//Erase
		HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

		spiInitTransaction(&mem->trans, NULL, txbuff);
		txbuff[0] = MEM_CMD_SSE;
		txbuff[1] = (sector_erase & 0xFF0000) >> 16;
		txbuff[2] = (sector_erase & 0xFF00) >> 8;
		txbuff[3] = (sector_erase & 0xFF);

		if (!spiNewTransferWait(&mem->spi, &mem->trans, 4, SPI_WRITE))
			break;

		HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

		rtos_Sleep_mS(MEM_SECTORERASE_TIMEMAX);

		//Check erase complete
		if (!nvsWaitReady(mem))
			break;

		if (sector == (num_sectors - 1))
			success = true;
	}

	return success;
}

/*----------------------------------------------------------------------*/

bool nvsVerify(MemObject *mem, size_t offset, void *source, size_t size)
{
	size_t length = size;
	size_t vsize;
	size_t voffset = offset;
	size_t source_offset = 0;
	uint8_t *source_char = (uint8_t *)source;

	while (length > 0)
	{
		if (length < MEM_PP_MAXBYTES)
			vsize = length;
		else
			vsize = MEM_PP_MAXBYTES;

		nvsReadTo(mem, voffset, mem->verify_buffer, vsize);

		for (size_t check = 0; check < vsize; check++)
		{
			if (mem->verify_buffer[check] != source_char[source_offset + check])
				return false;
		}

		length -= vsize;
		voffset += vsize;
		source_offset += vsize;
	}

	return true;
}

/*----------------------------------------------------------------------*/

bool nvsChipErase(MemObject *mem)
{
	//Write Enable
	if (!nvsWriteEnable(mem))
		return false;

	uint8_t txbuff = MEM_CMD_CHE;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

	spiInitTransaction(&mem->trans, NULL, &txbuff);

	if (!spiNewTransferWait(&mem->spi, &mem->trans, 1, SPI_WRITE))
		return false;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

	rtos_Sleep_mS(MEM_CHE_TIMEMAX);

	//Check erase complete
	if (!nvsWaitReady(mem))
		return false;

	return true;
}

/*----------------------------------------------------------------------*/

bool nvsWriteEnable(MemObject *mem)
{
	uint8_t txbuff = MEM_CMD_WREN;

	//Write Enable
	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

	spiInitTransaction(&mem->trans, NULL, &txbuff);

	if (!spiNewTransferWait(&mem->spi, &mem->trans, 1, SPI_WRITE))
		return false;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

	return true;
}

/*----------------------------------------------------------------------*/

bool nvsWriteDisable(MemObject *mem)
{
	uint8_t txbuff = MEM_CMD_WRDI;
	uint8_t status;

	bool success = false;
	for (uint8_t tries = 0; tries < 4; tries++)
	{
		HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

		spiInitTransaction(&mem->trans, NULL, &txbuff);

		if (!spiNewTransferWait(&mem->spi, &mem->trans, 1, SPI_WRITE))
			return false;

		HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

		rtos_Sleep_mS(1);

		if (!nvsReadStatus(mem, &status))
			break;

		if (status & (1 << MEM_STATBIT_WEN))
			continue;
		else
		{
			success = true;
			break;
		}
	}

	return success;
}

/*----------------------------------------------------------------------*/

bool nvsReadStatus(MemObject *mem, uint8_t *status)
{
	uint8_t txbuff = MEM_CMD_RDSR;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

	spiInitTransaction(&mem->trans, NULL, &txbuff);

	if (!spiNewTransferWait(&mem->spi, &mem->trans, 1, SPI_WRITE))
		return false;

	spiInitTransaction(&mem->trans, status, NULL);
	if (!spiNewTransferWait(&mem->spi, &mem->trans, 1, SPI_READ))
		return false;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

	rtos_Sleep_mS(1);

	return true;
}

/*----------------------------------------------------------------------*/

uint32_t nvsReadJID(MemObject *mem)
{
	uint8_t txbuff = MEM_CMD_RJID;

	uint32_t id = 0xFFFFFF;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

	spiInitTransaction(&mem->trans, NULL, &txbuff);

	if (!spiNewTransferWait(&mem->spi, &mem->trans, 1, SPI_WRITE))
		return id;

	spiInitTransaction(&mem->trans, &id, NULL);
	if (!spiNewTransferWait(&mem->spi, &mem->trans, 3, SPI_READ))
		return id;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

	rtos_Sleep_mS(10);

	return id;
}

/*----------------------------------------------------------------------*/

uint8_t nvsReadID(MemObject *mem)
{
	uint8_t txbuff = MEM_CMD_RID;

	uint32_t id = 0xFFFFFFFF;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

	spiInitTransaction(&mem->trans, NULL, &txbuff);

	if (!spiNewTransferWait(&mem->spi, &mem->trans, 1, SPI_WRITE))
		return id;

	spiInitTransaction(&mem->trans, &id, NULL);
	if (!spiNewTransferWait(&mem->spi, &mem->trans, 4, SPI_READ))
		return id;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

	rtos_Sleep_mS(10);

	return (uint8_t)((id & 0xFF000000) >> 24);
}

/*----------------------------------------------------------------------*/

bool nvsReset(MemObject *mem)
{
	uint8_t txbuff = MEM_CMD_RSTEN;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

	spiInitTransaction(&mem->trans, NULL, &txbuff);

	if (!spiNewTransferWait(&mem->spi, &mem->trans, 1, SPI_WRITE))
		return false;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

	rtos_Sleep_mS(1);

	txbuff = MEM_CMD_RST;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

	spiInitTransaction(&mem->trans, NULL, &txbuff);

	if (!spiNewTransferWait(&mem->spi, &mem->trans, 1, SPI_WRITE))
		return false;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

	rtos_Sleep_mS(50);

	return true;
}

/*----------------------------------------------------------------------*/

bool nvsWaitReady(MemObject *mem)
{
	uint8_t status;

	while (1)
	{
		rtos_Sleep_mS(1);

		if (!nvsReadStatus(mem, &status))
			return false;

		if (status & (1 << MEM_STATBIT_RDY))
			continue;
		else
			return true;
	}
}

/*----------------------------------------------------------------------*/

bool nvsEnterDeepPowerdown(MemObject *mem)
{
	uint8_t txbuff = MEM_CMD_DP;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

	spiInitTransaction(&mem->trans, NULL, &txbuff);

	if (!spiNewTransferWait(&mem->spi, &mem->trans, 1, SPI_WRITE))
		return false;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

	//Wait for tdp
	rtos_Sleep_mS(MEM_PD_TIME);

	return true;
}

/*----------------------------------------------------------------------*/

bool nvsExitDeepPowerdown(MemObject *mem)
{
	uint8_t txbuff = MEM_CMD_RID;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_RESET);

	spiInitTransaction(&mem->trans, NULL, &txbuff);

	if (!spiNewTransferWait(&mem->spi, &mem->trans, 1, SPI_WRITE))
		return false;

	HAL_GPIO_WritePin(mem->cs_port, mem->cs_pin, GPIO_PIN_SET);

	//Wait for trdp
	rtos_Sleep_mS(MEM_RPD_TIME);

	return true;
}
