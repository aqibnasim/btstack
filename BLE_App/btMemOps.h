/*! \file "btMemOps.h"
   \brief Contains handler functions for all characteristics in
   "Memory Operations" service

   This service is used to write data into our External NVS chip using the
   "mem_peri.h" peripheral interface.
   The service is an integral part of the OTA and device update functionality.
*/

#ifndef APPLICATION_BTMEMOPS_H_
#define APPLICATION_BTMEMOPS_H_

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "memops_service.h"

#include "memory/mem_peri.h"


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CONSTANTS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\var uint8_t MEMOPS_MSGTYPES_ERASESECTOR
	Number of Message-types in Characteristic
*/
static const uint8_t MEMOPS_MSGTYPES_ERASESECTOR = 2;

/*!\var uint8_t MEMOPS_MSGTYPES_ERASECHUNK
	Number of Message-types in Characteristic
*/
static const uint8_t MEMOPS_MSGTYPES_ERASECHUNK = 2;

/*!\var uint8_t MEMOPS_MSGTYPES_ADDELEMENT
	Number of Message-types in Characteristic
*/
static const uint8_t MEMOPS_MSGTYPES_ADDELEMENT = 2;

/*!\var uint8_t MEMOPS_MSGTYPES_WRITENVS
	Number of Message-types in Characteristic
*/
static const uint8_t MEMOPS_MSGTYPES_WRITENVS = 6;

/*!\var uint8_t MEMOPS_MSGTYPES_WRITEELEMENT
	Number of Message-types in Characteristic
*/
static const uint8_t MEMOPS_MSGTYPES_WRITEELEMENT = 6;

/*!\var uint8_t MEMOPS_MSGTYPES_SOFTRESET
	Number of Message-types in Characteristic
*/
static const uint8_t MEMOPS_MSGTYPES_SOFTRESET = 1;

/*!\var uint8_t MEMOPS_MSGTYPES_CHECKHASH
	Number of Message-types in Characteristic
*/
static const uint8_t MEMOPS_MSGTYPES_CHECKHASH = 4;

static const uint8_t MEMOPS_NOPROCESS = 0xFF;

/*!\var uint8_t MEMOPS_ALL_ACKERR_POS
	Index of Acknowledgement/Error Byte in incoming message
*/
static const uint8_t MEMOPS_ALL_ACKERR_POS = 1;

//PDU * Verify Size can't be more than MEM_BUFF_SIZE
/*!\var uint8_t MEMOPS_PDU_SIZE
	Maximum size of Service data packet
*/
static const uint8_t MEMOPS_PDU_SIZE = 16;                      //Odd numbers seem to be problematic

/*!\var uint8_t MEMOPS_DATA_VERIFYSIZE
	Amount of data to read and verify during large sequential transactions
*/
static const uint16_t MEMOPS_DATA_VERIFYSIZE = 32;

/*!\var uint8_t MEMOPS_DATA_VERIFYLAST
	Value that indicates last packet to verify
*/
static const uint16_t MEMOPS_DATA_VERIFYLAST = 0xFFFF;

//WriteToNVS
static const uint8_t MEMOPS_WRITENVS_1_LEN = 9;
static const uint8_t MEMOPS_WRITENVS_2_LEN = 5;
static const uint8_t MEMOPS_WRITENVS_3_LEN = 8;
static const uint8_t MEMOPS_WRITENVS_4_LEN = 9;
static const uint8_t MEMOPS_WRITENVS_5_LEN = 20;
static const uint8_t MEMOPS_WRITENVS_6_LEN = 8;

static const uint8_t MEMOPS_WRITENVS_1OFFSET_POS = 1;
static const uint8_t MEMOPS_WRITENVS_1SIZE_POS = 5;
static const uint8_t MEMOPS_WRITENVS_2PDU_POS = 2;
static const uint8_t MEMOPS_WRITENVS_2VERIFY_POS = 3;
static const uint8_t MEMOPS_WRITENVS_3PDU_POS = 1;
static const uint8_t MEMOPS_WRITENVS_3VERIFY_POS = 2;
static const uint8_t MEMOPS_WRITENVS_3OFFSET_POS = 4;
static const uint8_t MEMOPS_WRITENVS_4PDU_POS = 2;
static const uint8_t MEMOPS_WRITENVS_4VERIFY_POS = 3;
static const uint8_t MEMOPS_WRITENVS_4OFFSET_POS = 5;
static const uint8_t MEMOPS_WRITENVS_5VCNT_POS = 1;
static const uint8_t MEMOPS_WRITENVS_5DATA_POS = 3;
static const uint8_t MEMOPS_WRITENVS_6VCNT_POS = 2;
static const uint8_t MEMOPS_WRITENVS_6OFFSET_POS = 4;

//WriteToElement
static const uint8_t MEMOPS_WRITEELEM_1_LEN = 12;
static const uint8_t MEMOPS_WRITEELEM_2_LEN = 5;
static const uint8_t MEMOPS_WRITEELEM_3_LEN = 8;
static const uint8_t MEMOPS_WRITEELEM_4_LEN = 9;
static const uint8_t MEMOPS_WRITEELEM_5_LEN = 20;
static const uint8_t MEMOPS_WRITEELEM_6_LEN = 8;

static const uint8_t MEMOPS_WRITEELEM_1CHUNK_POS = 1;
static const uint8_t MEMOPS_WRITEELEM_1ELEM_POS = 2;
static const uint8_t MEMOPS_WRITEELEM_1SIZE_POS = 4;
static const uint8_t MEMOPS_WRITEELEM_1OFFSET_POS = 8;
static const uint8_t MEMOPS_WRITEELEM_2PDU_POS = 2;
static const uint8_t MEMOPS_WRITEELEM_2VERIFY_POS = 3;
static const uint8_t MEMOPS_WRITEELEM_3PDU_POS = 1;
static const uint8_t MEMOPS_WRITEELEM_3VERIFY_POS = 2;
static const uint8_t MEMOPS_WRITEELEM_3OFFSET_POS = 4;
static const uint8_t MEMOPS_WRITEELEM_4PDU_POS = 2;
static const uint8_t MEMOPS_WRITEELEM_4VERIFY_POS = 3;
static const uint8_t MEMOPS_WRITEELEM_4OFFSET_POS = 5;
static const uint8_t MEMOPS_WRITEELEM_5VCNT_POS = 1;
static const uint8_t MEMOPS_WRITEELEM_5DATA_POS = 3;
static const uint8_t MEMOPS_WRITEELEM_6VCNT_POS = 2;
static const uint8_t MEMOPS_WRITEELEM_6OFFSET_POS = 4;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS ------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\enum btMemOpsInterface
 Interface used in communication
*/
typedef enum
{
	MEMOPS_UART,
	MEMOPS_BLE,
} btMemOpsInterface;

/*!\enum btMemOpsStatus
 List of function status returns
*/
typedef enum
{
    MEMOPS_STAT_SUCCESS = 0,
    MEMOPS_STAT_FAIL,
    MEMOPS_STAT_INVALIDTYPE,
    MEMOPS_STAT_INVALIDPARAM,
    MEMOPS_STAT_PROCESSBUSY
} btMemOpsStatus;

/*!\enum btMemOpsResponses
 List of responses to send back to Host
*/
typedef enum
{
    MEMOPS_RESP_SUCCESS = 0,
    MEMOPS_RESP_CHUNKINVALID,
    MEMOPS_RESP_ELEMSIZEINVALID,
    MEMOPS_RESP_OFFSETINVALID,
    MEMOPS_RESP_OVERFLOW,
    MEMOPS_RESP_PACKETLOSS,
    MEMOPS_RESP_SECTORINVALID,
    MEMOPS_RESP_SECTORUSED,
    MEMOPS_RESP_CHUNKCONFLICT
} btMemOpsResponses;

/*!\typedef btMemOpsState
 Contains the state of the Service. Used to store data during
 multi-message sequential communication.

 \var uint8_t btMemOpsState::active_process
 The Process/Characteristic that is currently active
 \var btMemOpsInterface btMemOpsState::interface
 Interface used in communication
 \var size_t btMemOpsState::size
 Stored size of incoming data
 \var size_t btMemOpsState::offset
 Stored offset of incoming data
 \var size_t btMemOpsState::buff_offset
 Offset of buffer
 \var MemChunk btMemOpsState::*chunk
 Stored pointer to working Chunk
 \var MemElement btMemOpsState::elem
 Working element
 \var uint16_t btMemOpsState::verify_count
 Verify counter, used during large transfers
 \var uint8_t btMemOpsState::rx_oad_hash
 Received hash to check against
 \var bool btMemOpsState::error
 Error in transfer
*/
typedef struct
{
    uint8_t active_process;
    btMemOpsInterface interface;

    size_t size;
    size_t offset;
    size_t buff_offset;
    MemChunk *chunk;
    MemElement elem;
    uint16_t verify_count;

    uint8_t rx_oad_hash[32];

    bool error;
} btMemOpsState;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Handles input data from Service

	\param interface Interface used in communication
    \param *state Pointer to mainState object
    \param *pCharData Incoming data
    \param charId Characteristic of incoming data
    \return Service status
*/
btMemOpsStatus btMemOpsService_Handler(btMemOpsInterface interface, MemObject *mem, uint8_t *pCharData, uint8_t charId);


#endif /* APPLICATION_BTMEMOPS_H_ */
