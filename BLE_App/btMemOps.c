/*! \file "btMemOps.c"
   \brief Contains handler functions for all characteristics in
   "Memory Operations" service

   This service is used to write data into our External NVS chip using the
   "mem_peri.h" peripheral interface.
   The service is an integral part of the OTA and device update functionality.
*/

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "btMemOps.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Handles incoming data for Characteristic btMemOps/EraseSector

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btMemOpsStatus btMemOpsEraseSectorHandler(MemObject *mem, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btMemOps/EraseChunk

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btMemOpsStatus btMemOpsEraseChunkHandler(MemObject *mem, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btMemOps/AddElement

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btMemOpsStatus btMemOpsAddElementHandler(MemObject *mem, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btMemOps/WriteToNVS

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btMemOpsStatus btMemOpsWriteToNVSHandler(MemObject *mem, uint8_t *pCharData);

/*! \brief Handles incoming data for Characteristic btMemOps/WriteToElement

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btMemOpsStatus btMemOpsWriteToElementHandler(MemObject *mem, uint8_t *pCharData);

/*! \brief Handles request for Soft Reset

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btMemOpsStatus btMemOpsSoftResetHandler(MemObject *mem, uint8_t *pCharData);

/*! \brief Handles request to Check HASH of OAD image

    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \return Service status
*/
static btMemOpsStatus btMemOpsCheckHashHandler(MemObject *mem, uint8_t *pCharData);

/*! \brief Extract the message type

    \param *pCharData Incoming data
	\param *type_return Type of message
*/
static void btMemOpsUtilExtractMsgType(uint8_t *pCharData, uint8_t *type_return);

/*! \brief Check if a given NVS offset falls within any of the chunks
 	 defined on the external NVS

    \param *mem Pointer to MemObject containing external NVS details
	\param offset Type of message
	\return Does the offset lie in a chunk?
*/
static bool btMemOpsUtilOffsetInChunk(MemObject *mem, size_t offset);

/*! \brief Check if a write command with a specified start and end offset
 	 is valid

    \param *mem Pointer to MemObject containing external NVS details
	\param start Start offset of write
	\param end End offset of write
	\return Write Validity
*/
static btMemOpsResponses btMemOpsUtilWriteValid(MemObject *mem, size_t start, size_t end);

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
static bool MemOpsService_TX(btMemOpsInterface interface, uint8_t param, uint16_t len, void *value);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* LOCAL VARIABLES -----------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \var btMemOpsState ops_state
  Contains the state of this service.
  Used to keep track of multi-message transfers
*/
btMemOpsState *ops_state;

btMemOpsState ops_state_uart;
btMemOpsState ops_state_ble;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* MEMORY OPERATION TX FUNCTION ----------------------------------------*/
/*----------------------------------------------------------------------*/

bool MemOpsService_TX(btMemOpsInterface interface, uint8_t param, uint16_t len, void *value)
{
	if (interface == MEMOPS_UART)
		return MemOpsService_SetParameter_UART(param, len, value);
	else if (interface == MEMOPS_BLE)
		return false;
//		return MemOpsService_SetParameter_BLE(param, len, value);
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* MEMORY OPERATION CHARACTERISTIC HANDLER FUNCTIONS -------------------*/
/*----------------------------------------------------------------------*/

btMemOpsStatus btMemOpsService_Handler(btMemOpsInterface interface, MemObject *mem,
		uint8_t *pCharData, uint8_t charId)
{
	if (interface == MEMOPS_UART)
		ops_state = &ops_state_uart;
	else if (interface == MEMOPS_BLE)
		ops_state = &ops_state_ble;

	ops_state->interface = interface;

    switch (charId)
    {
    case BTMEM_ERASESECTOR_ID:
        return btMemOpsEraseSectorHandler(mem, pCharData);

    case BTMEM_ERASECHUNK_ID:
        return btMemOpsEraseChunkHandler(mem, pCharData);

    case BTMEM_ADDELEMENT_ID:
        return btMemOpsAddElementHandler(mem, pCharData);

    case BTMEM_WRITENVS_ID:
        return btMemOpsWriteToNVSHandler(mem, pCharData);

    case BTMEM_WRITEELEMENT_ID:
        return btMemOpsWriteToElementHandler(mem, pCharData);

    case BTMEM_SOFTRESET_ID:
    	return btMemOpsSoftResetHandler(mem, pCharData);

    case BTMEM_CHECKHASH_ID:
    	return btMemOpsCheckHashHandler(mem, pCharData);
    default:
      return MEMOPS_STAT_FAIL;
    }
}

/*----------------------------------------------------------------------*/
//Characteristic: ERASE SECTOR
btMemOpsStatus btMemOpsEraseSectorHandler(MemObject *mem, uint8_t *pCharData)
{
    //Extract message type
    uint8_t msg_type;
    btMemOpsUtilExtractMsgType(pCharData, &msg_type);

    //Filter types
    if (msg_type > MEMOPS_MSGTYPES_ERASESECTOR)
        return MEMOPS_STAT_INVALIDTYPE;

    uint16_t sector;
    memcpy(&sector, pCharData+1, sizeof(uint16_t));

    uint8_t resp[BTMEM_ERASESECTOR_LEN];
    resp[0] = 2;
    memcpy(resp+2, pCharData+1, sizeof(uint16_t));

    switch (msg_type)
    {
    case 1:
        if (sector >= (mem->totalSize / mem->sectorSize))
        {
            resp[MEMOPS_ALL_ACKERR_POS] = MEMOPS_RESP_SECTORINVALID;
            MemOpsService_TX(ops_state->interface, BTMEM_ERASESECTOR_ID, 4, resp);
            return MEMOPS_STAT_INVALIDPARAM;
        }
        else
        {
            size_t sector_offset = sector * mem->sectorSize;
            if (btMemOpsUtilOffsetInChunk(mem, sector_offset))
            {
                resp[MEMOPS_ALL_ACKERR_POS] = MEMOPS_RESP_SECTORUSED;
                MemOpsService_TX(ops_state->interface, BTMEM_ERASESECTOR_ID, 4, resp);
                return MEMOPS_STAT_INVALIDPARAM;
            }

            resp[MEMOPS_ALL_ACKERR_POS] = MEMOPS_RESP_SUCCESS;
            MemOpsService_TX(ops_state->interface, BTMEM_ERASESECTOR_ID, 4, resp);

            nvsErase(mem, sector_offset, 1);

            return MEMOPS_STAT_SUCCESS;
        }
    default:
        return MEMOPS_STAT_INVALIDTYPE;
    }
}

/*----------------------------------------------------------------------*/
//Characteristic: ERASE CHUNK
btMemOpsStatus btMemOpsEraseChunkHandler(MemObject *mem, uint8_t *pCharData)
{
    //Extract message type
    uint8_t msg_type;
    btMemOpsUtilExtractMsgType(pCharData, &msg_type);

    //Filter types
    if (msg_type > MEMOPS_MSGTYPES_ERASECHUNK)
        return MEMOPS_STAT_INVALIDTYPE;

    uint8_t chunk_index = pCharData[1];
    uint8_t resp[BTMEM_ERASESECTOR_LEN];
    resp[0] = 2;
    resp[2] = chunk_index;

    switch (msg_type)
    {
    case 1:
        if (chunk_index >= mem->num_chunks)
        {
            resp[MEMOPS_ALL_ACKERR_POS] = MEMOPS_RESP_CHUNKINVALID;
            MemOpsService_TX(ops_state->interface, BTMEM_ERASECHUNK_ID, 3, resp);
            return MEMOPS_STAT_INVALIDPARAM;
        }
        else
        {
            resp[MEMOPS_ALL_ACKERR_POS] = MEMOPS_RESP_SUCCESS;
            MemOpsService_TX(ops_state->interface, BTMEM_ERASECHUNK_ID, 3, resp);

            memInitChunk(mem, &mem->chunks[chunk_index]);

            return MEMOPS_STAT_SUCCESS;
        }
    default:
        return MEMOPS_STAT_INVALIDTYPE;
    }
}

/*----------------------------------------------------------------------*/
//Characteristic: ADD ELEMENT
btMemOpsStatus btMemOpsAddElementHandler(MemObject *mem, uint8_t *pCharData)
{
    //Extract message type
    uint8_t msg_type;
    btMemOpsUtilExtractMsgType(pCharData, &msg_type);

    //Filter types
    if (msg_type > MEMOPS_MSGTYPES_ADDELEMENT)
        return MEMOPS_STAT_INVALIDTYPE;

    uint8_t chunk_index = pCharData[1];
    size_t elem_size;
    memcpy(&elem_size, pCharData+2, sizeof(size_t));

    uint8_t resp[BTMEM_ADDELEMENT_LEN];
    resp[0] = 2;
    resp[2] = chunk_index;
    memcpy(resp+3, pCharData+2, sizeof(size_t));

    switch (msg_type)
    {
    case 1:
        if (chunk_index >= mem->num_chunks)
        {
            resp[MEMOPS_ALL_ACKERR_POS] = MEMOPS_RESP_CHUNKINVALID;
            MemOpsService_TX(ops_state->interface, BTMEM_ADDELEMENT_ID, 9, resp);
            return MEMOPS_STAT_INVALIDPARAM;
        }
        else
        {
            MemElement elem;
            uint16_t elemindex;
            if (memAddElement(mem, &mem->chunks[chunk_index], elem_size, &elem, &elemindex) != MEM_OP_SUCCESS)
            {
                resp[MEMOPS_ALL_ACKERR_POS] = MEMOPS_RESP_ELEMSIZEINVALID;
                MemOpsService_TX(ops_state->interface, BTMEM_ADDELEMENT_ID, 9, resp);
                return MEMOPS_STAT_INVALIDPARAM;
            }
            else
            {
                resp[MEMOPS_ALL_ACKERR_POS] = MEMOPS_RESP_SUCCESS;
                memcpy(resp+7, &elemindex, sizeof(uint16_t));
                MemOpsService_TX(ops_state->interface, BTMEM_ADDELEMENT_ID, 9, resp);
                return MEMOPS_STAT_SUCCESS;
            }
        }
    default:
        return MEMOPS_STAT_INVALIDTYPE;
    }
}

/*----------------------------------------------------------------------*/
//Characteristic: WRITE TO NVS
btMemOpsStatus btMemOpsWriteToNVSHandler(MemObject *mem, uint8_t *pCharData)
{
    //Extract message type
    uint8_t msg_type;
    btMemOpsUtilExtractMsgType(pCharData, &msg_type);

    //Filter types
    if (msg_type > MEMOPS_MSGTYPES_WRITENVS)
        return MEMOPS_STAT_INVALIDTYPE;


    uint8_t resp[BTMEM_WRITENVS_LEN];

    switch (msg_type)
    {
    case 1:
        //Prepare response
        resp[0] = 2;
        memcpy(resp+MEMOPS_WRITENVS_2PDU_POS, &MEMOPS_PDU_SIZE, sizeof(uint8_t));
        memcpy(resp+MEMOPS_WRITENVS_2VERIFY_POS, &MEMOPS_DATA_VERIFYSIZE, sizeof(uint16_t));

        //Extract data
        size_t *offset = (size_t *)(pCharData + MEMOPS_WRITENVS_1OFFSET_POS);
        size_t *size = (size_t *)(pCharData + MEMOPS_WRITENVS_1SIZE_POS);
        size_t end_offset = *offset + *size;

        //Check validity of Offset
        btMemOpsResponses check = btMemOpsUtilWriteValid(mem, *offset, end_offset);
        memcpy(resp+MEMOPS_ALL_ACKERR_POS, &check, sizeof(uint8_t));
        MemOpsService_TX(ops_state->interface, BTMEM_WRITENVS_ID, MEMOPS_WRITENVS_2_LEN, resp);

        if (check != MEMOPS_RESP_SUCCESS)
        {
            //Error
            ops_state->active_process = MEMOPS_NOPROCESS;
            return MEMOPS_STAT_INVALIDPARAM;
        }

        //Success
        ops_state->offset = *offset;
        ops_state->size = *size;
        ops_state->error = false;
        ops_state->active_process = BTMEM_WRITENVS_ID;
        return MEMOPS_STAT_SUCCESS;

    case 3:
        if (ops_state->active_process != BTMEM_WRITENVS_ID)
            return MEMOPS_STAT_PROCESSBUSY;

        //Prepare response
        resp[0] = 4;
        resp[1] = 0;

        uint16_t *verify_size = (uint16_t *)(pCharData + MEMOPS_WRITENVS_3VERIFY_POS);
        if (*verify_size != MEMOPS_DATA_VERIFYSIZE)
        {
            ops_state->active_process = MEMOPS_NOPROCESS;
            return MEMOPS_STAT_INVALIDPARAM;
        }

        ops_state->offset = *((size_t*)(pCharData + MEMOPS_WRITENVS_3OFFSET_POS));
        ops_state->verify_count = 0;
        ops_state->error = false;

        memcpy(resp + MEMOPS_WRITENVS_4PDU_POS,
                   &MEMOPS_PDU_SIZE, sizeof(uint8_t));
        memcpy(resp + MEMOPS_WRITENVS_4VERIFY_POS,
                   verify_size, sizeof(uint16_t));
        memcpy(resp + MEMOPS_WRITENVS_4OFFSET_POS,
                   &ops_state->offset, sizeof(size_t));

        MemOpsService_TX(ops_state->interface, BTMEM_WRITENVS_ID, MEMOPS_WRITENVS_4_LEN, resp);
        return MEMOPS_STAT_SUCCESS;

    case 5:
        if (ops_state->active_process != BTMEM_WRITENVS_ID)
            return MEMOPS_STAT_PROCESSBUSY;

        uint16_t *vcount = (uint16_t*)(pCharData + MEMOPS_WRITEELEM_6VCNT_POS);

        //Check if message count is what we expect
        if (*vcount != ops_state->verify_count && *vcount != MEMOPS_DATA_VERIFYLAST)
            ops_state->error = true;

        bool end_reached = true;
        size_t write_size = MEMOPS_PDU_SIZE;

        if (((*vcount+1) * MEMOPS_PDU_SIZE) >= ops_state->size)
        {
            write_size = ops_state->size - (*vcount * MEMOPS_PDU_SIZE);
        }
        else if (*vcount == MEMOPS_DATA_VERIFYSIZE-1)
        {
            write_size = MEMOPS_PDU_SIZE;
        }
        else
            end_reached = false;

        if (!ops_state->error)
        {
            nvsWriteFrom(mem, ops_state->offset,
                         (void*)(pCharData + MEMOPS_WRITENVS_5DATA_POS), write_size);

            ops_state->offset += write_size;
            ops_state->verify_count = *vcount;
        }

        //Check if verify counter has hit its limit. If so send SUCC/ERR
        if (end_reached)
        {
            resp[0] = 6;
            if (ops_state->error)
                resp[MEMOPS_ALL_ACKERR_POS] = MEMOPS_RESP_PACKETLOSS;
            else
                resp[MEMOPS_ALL_ACKERR_POS] = MEMOPS_RESP_SUCCESS;

            memcpy(resp + MEMOPS_WRITENVS_6VCNT_POS,
                   &ops_state->verify_count, sizeof(uint16_t));
            memcpy(resp + MEMOPS_WRITENVS_6OFFSET_POS,
                   &ops_state->offset, sizeof(size_t));

            MemOpsService_TX(ops_state->interface, BTMEM_WRITENVS_ID, MEMOPS_WRITENVS_6_LEN, resp);
        }
        return MEMOPS_STAT_SUCCESS;
    default:
        return MEMOPS_STAT_INVALIDTYPE;
    }
}

/*----------------------------------------------------------------------*/
//Characteristic: WRITE TO ELEMENT
btMemOpsStatus btMemOpsWriteToElementHandler(MemObject *mem, uint8_t *pCharData)
{
    //Extract message type
    uint8_t msg_type;
    btMemOpsUtilExtractMsgType(pCharData, &msg_type);

    //Filter types
    if (msg_type > MEMOPS_MSGTYPES_WRITEELEMENT)
        return MEMOPS_STAT_INVALIDTYPE;

    uint8_t resp[BTMEM_WRITEELEMENT_LEN];

    switch (msg_type)
    {
    case 1:
        //Prepare response
        resp[0] = 2;
        memcpy(resp+MEMOPS_WRITEELEM_2PDU_POS, &MEMOPS_PDU_SIZE, sizeof(uint8_t));
        memcpy(resp+MEMOPS_WRITEELEM_2VERIFY_POS, &MEMOPS_DATA_VERIFYSIZE, sizeof(uint16_t));

        //Extract data
        uint8_t *chunk_index = (uint8_t *)(pCharData + MEMOPS_WRITEELEM_1CHUNK_POS);
        uint16_t *elem_index = (uint16_t *)(pCharData + MEMOPS_WRITEELEM_1ELEM_POS);
        size_t *offset = (size_t *)(pCharData + MEMOPS_WRITEELEM_1OFFSET_POS);
        size_t *size = (size_t *)(pCharData + MEMOPS_WRITEELEM_1SIZE_POS);
        size_t end_offset = *offset + *size;
        uint8_t check = MEMOPS_RESP_SUCCESS;

        //Check validity of Chunk index
        if (*chunk_index >= mem->num_chunks)
            check = MEMOPS_RESP_CHUNKINVALID;
        else
        {
           ops_state->chunk = &mem->chunks[*chunk_index];
           //Check Element index
           if (memReadElementParam(mem, ops_state->chunk, &ops_state->elem, *elem_index) != MEM_OP_SUCCESS)
               check = MEMOPS_RESP_ELEMSIZEINVALID;
           else
           {
               //Check Offset
               if (*offset >= ops_state->elem.size)
                   check = MEMOPS_RESP_OFFSETINVALID;
               else if (end_offset > ops_state->elem.size)
                   check = MEMOPS_RESP_OFFSETINVALID;
           }
        }

        memcpy(resp+MEMOPS_ALL_ACKERR_POS, &check, sizeof(uint8_t));
        MemOpsService_TX(ops_state->interface, BTMEM_WRITEELEMENT_ID, MEMOPS_WRITEELEM_2_LEN, resp);

        if (check != MEMOPS_RESP_SUCCESS)
        {
            //Error
            ops_state->active_process = MEMOPS_NOPROCESS;
            return MEMOPS_STAT_INVALIDPARAM;
        }

        //Success
        ops_state->offset = *offset;
        ops_state->size = *size;
        ops_state->error = false;
        ops_state->active_process = BTMEM_WRITEELEMENT_ID;
        return MEMOPS_STAT_SUCCESS;

    case 3:
        if (ops_state->active_process != BTMEM_WRITEELEMENT_ID)
            return MEMOPS_STAT_PROCESSBUSY;

        //Prepare response
        resp[0] = 4;
        resp[1] = 0;

        uint16_t *verify_size = (uint16_t *)(pCharData + MEMOPS_WRITEELEM_3VERIFY_POS);
        if (*verify_size != MEMOPS_DATA_VERIFYSIZE)
        {
            ops_state->active_process = MEMOPS_NOPROCESS;
            return MEMOPS_STAT_INVALIDPARAM;
        }

        ops_state->offset = *((size_t*)(pCharData + MEMOPS_WRITEELEM_3OFFSET_POS));
        ops_state->buff_offset = 0;
        ops_state->verify_count = 0;
        ops_state->error = false;

        memcpy(resp + MEMOPS_WRITEELEM_4PDU_POS,
                   &MEMOPS_PDU_SIZE, sizeof(uint8_t));
        memcpy(resp + MEMOPS_WRITEELEM_4VERIFY_POS,
                   verify_size, sizeof(uint16_t));
        memcpy(resp + MEMOPS_WRITEELEM_4OFFSET_POS,
                   &ops_state->offset, sizeof(size_t));

        MemOpsService_TX(ops_state->interface, BTMEM_WRITEELEMENT_ID, MEMOPS_WRITEELEM_4_LEN, resp);
        return MEMOPS_STAT_SUCCESS;

    case 5:
        if (ops_state->active_process != BTMEM_WRITEELEMENT_ID)
            return MEMOPS_STAT_PROCESSBUSY;

        uint16_t *vcount = (uint16_t*)(pCharData + MEMOPS_WRITEELEM_5VCNT_POS);

        //Check if message count is what we expect
        if (*vcount != ops_state->verify_count && *vcount != MEMOPS_DATA_VERIFYLAST)
        {
            ops_state->error = true;
        }

        bool end_reached = false;
        if (*vcount == MEMOPS_DATA_VERIFYLAST)
            end_reached = true;

        size_t write_size = MEMOPS_PDU_SIZE;
        if ((ops_state->offset + ops_state->buff_offset + MEMOPS_PDU_SIZE) >= ops_state->elem.size)
        {
            write_size = ops_state->elem.size - ops_state->offset - ops_state->buff_offset;
        }

        if (!ops_state->error)
        {
            memcpy(mem->buffer + ops_state->buff_offset, (void*)(pCharData + MEMOPS_WRITEELEM_5DATA_POS), write_size);

            /*ops_state->offset += write_size;*/
            ops_state->buff_offset += write_size;
            ops_state->verify_count++;
        }

        //Check if verify counter has hit its limit. If so send SUCC/ERR
        if (end_reached)
        {
            resp[0] = 6;
            if (ops_state->error)
                resp[MEMOPS_ALL_ACKERR_POS] = MEMOPS_RESP_PACKETLOSS;
            else
                resp[MEMOPS_ALL_ACKERR_POS] = MEMOPS_RESP_SUCCESS;

            size_t curr_offset = ops_state->offset + ops_state->buff_offset;

            memcpy(resp + MEMOPS_WRITEELEM_6VCNT_POS,
                   &ops_state->verify_count, sizeof(uint16_t));
            memcpy(resp + MEMOPS_WRITEELEM_6OFFSET_POS,
                   &curr_offset, sizeof(size_t));

            MemOpsService_TX(ops_state->interface, BTMEM_WRITEELEMENT_ID, MEMOPS_WRITEELEM_6_LEN, resp);

            //Write frame to Memory
            memWriteToElement(mem, ops_state->chunk, &ops_state->elem, ops_state->offset,
                  mem->buffer, ops_state->buff_offset);

            ops_state->offset += ops_state->buff_offset;
        }
        return MEMOPS_STAT_SUCCESS;
    default:
        return MEMOPS_STAT_INVALIDTYPE;
    }
}

/*----------------------------------------------------------------------*/
//Characteristic: SOFT RESET
btMemOpsStatus btMemOpsSoftResetHandler(MemObject *mem, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btMemOpsUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type > MEMOPS_MSGTYPES_SOFTRESET)
		return MEMOPS_STAT_INVALIDTYPE;

	switch (msg_type)
	{
	case 1:
		smQueueSoftReset(&main_state);
        return MEMOPS_STAT_SUCCESS;
	default:
		return MEMOPS_STAT_INVALIDTYPE;
	}
}

/*----------------------------------------------------------------------*/
//Characteristic: CHECK HASH
btMemOpsStatus btMemOpsCheckHashHandler(MemObject *mem, uint8_t *pCharData)
{
	//Extract message type
	uint8_t msg_type;
	btMemOpsUtilExtractMsgType(pCharData, &msg_type);

	//Filter types
	if (msg_type > MEMOPS_MSGTYPES_CHECKHASH)
		return MEMOPS_STAT_INVALIDTYPE;

    uint8_t resp[BTMEM_WRITEELEMENT_LEN];

	switch (msg_type)
	{
	case 1:
		for (uint8_t i = 0; i < 16; i++)
			ops_state->rx_oad_hash[i] = pCharData[1 + i];
		return MEMOPS_STAT_SUCCESS;
	case 2:
		for (uint8_t i = 0; i < 16; i++)
			ops_state->rx_oad_hash[i + 16] = pCharData[1 + i];
		return MEMOPS_STAT_SUCCESS;
	case 3:
		if (memCheckChunk(mem, &mem->chunks[pCharData[1]]) != MEM_OP_SUCCESS)
			return MEMOPS_STAT_INVALIDPARAM;

		MemChunk *chunk2check = &mem->chunks[pCharData[1]];
		uint8_t result = memCheckHashOAD(mem, chunk2check, ops_state->rx_oad_hash);

		resp[0] = 4;
		resp[1] = result;

		MemOpsService_TX(ops_state->interface, BTMEM_CHECKHASH_ID, 2, resp);

		return MEMOPS_STAT_SUCCESS;
	default:
		return MEMOPS_STAT_INVALIDTYPE;
	}
}

/*----------------------------------------------------------------------*/

void btMemOpsUtilExtractMsgType(uint8_t *pCharData, uint8_t *type_return)
{
    *type_return = pCharData[0];
}

/*----------------------------------------------------------------------*/

bool btMemOpsUtilOffsetInChunk(MemObject *mem, size_t offset)
{
    for (uint8_t i = 0; i < mem->num_chunks; i++)
    {
        if (offset >= mem->chunks[i].offset &&
                offset < (mem->chunks[i].offset + (mem->chunks[i].num_sectors * mem->sectorSize)))
        {
            return true;
        }
    }

    return false;
}

/*----------------------------------------------------------------------*/

btMemOpsResponses btMemOpsUtilWriteValid(MemObject *mem, size_t start, size_t end)
{
    //Check if we overflow
    if (start >= mem->totalSize || end >= mem->totalSize)
        return MEMOPS_RESP_OVERFLOW;

    for (uint8_t i = 0; i < mem->num_chunks; i++)
    {
        //Do we start or end in a chunk
        if (btMemOpsUtilOffsetInChunk(mem, start))
            return MEMOPS_RESP_OFFSETINVALID;

        if (btMemOpsUtilOffsetInChunk(mem, end))
            return MEMOPS_RESP_OVERFLOW;

        //Does a chunk fall within write area
        if (mem->chunks[i].offset > start &&
                (mem->chunks[i].offset + (mem->chunks[i].num_sectors * mem->sectorSize)) <= end)
            return MEMOPS_RESP_OVERFLOW;

    }

    return true;
}
