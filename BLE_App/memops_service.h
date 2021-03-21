/*! \file "memops_service.h"
   \brief Contains details for Service and Characteristics

   Function for Parameter setting
*/

#ifndef PROFILES_MEMOPS_SERVICE_H_
#define PROFILES_MEMOPS_SERVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CONSTANTS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\def DEVCTRL_UUID_BASE128
	128bit UUID
*/
#define MEMOPS_UUID_BASE128(uuid) 0x1B, 0x3A, 0xB5, 0x07, 0x55, 0xC1, \
    0xBC, 0xB2, 0xF7, 0x4A, 0x7B, 0x84, \
    LO_UINT16(uuid), HI_UINT16(uuid), 0x17, 0xA3

/*!\def MEMOPS_SERV_UUID
	4bit Service UUID
*/
#define MEMOPS_SERV_UUID 0x99E0

// CHARACTERISTIC Details
// btMemEraseSector
#define BTMEM_ERASESECTOR_ID         0
#define BTMEM_ERASESECTOR_UUID       0x99E1
#define BTMEM_ERASESECTOR_LEN        4
#define BTMEM_ERASESECTOR_LEN_MIN    3

// btMemEraseChunk
#define BTMEM_ERASECHUNK_ID         1
#define BTMEM_ERASECHUNK_UUID       0x99E2
#define BTMEM_ERASECHUNK_LEN        3
#define BTMEM_ERASECHUNK_LEN_MIN    2

// btMemAddElement
#define BTMEM_ADDELEMENT_ID         2
#define BTMEM_ADDELEMENT_UUID       0x99E3
#define BTMEM_ADDELEMENT_LEN        9
#define BTMEM_ADDELEMENT_LEN_MIN    6

// btMemWriteToNVS
#define BTMEM_WRITENVS_ID           3
#define BTMEM_WRITENVS_UUID         0x99E4
#define BTMEM_WRITENVS_LEN          20
#define BTMEM_WRITENVS_LEN_MIN      5

// btMemWriteToElement
#define BTMEM_WRITEELEMENT_ID         4
#define BTMEM_WRITEELEMENT_UUID       0x99E5
#define BTMEM_WRITEELEMENT_LEN        20
#define BTMEM_WRITEELEMENT_LEN_MIN    5

// btMemSoftReset
#define BTMEM_SOFTRESET_ID         5
#define BTMEM_SOFTRESET_UUID       0x99E6
#define BTMEM_SOFTRESET_LEN        2
#define BTMEM_SOFTRESET_LEN_MIN    1

// btMemCheckHash
#define BTMEM_CHECKHASH_ID         6
#define BTMEM_CHECKHASH_UUID       0x99E7
#define BTMEM_CHECKHASH_LEN        17
#define BTMEM_CHECKHASH_LEN_MIN    2

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* API FUNCTIONS -------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Set a MemOpsService parameter over BLE

    \param param Profile parameter ID
    \param len length of data to write
    \param *value pointer to data to write.  This is dependent on
             the parameter ID and may be cast to the appropriate
             data type (example: data type of uint16_t will be cast to
             uint16_t pointer).

    \return Success
*/
extern bool MemOpsService_SetParameter_BLE(uint8_t param,
                                          uint16_t len,
                                          void *value);

/*! \brief Set a MemOpsService parameter over UART

    \param param Profile parameter ID
    \param len length of data to write
    \param *value pointer to data to write.  This is dependent on
             the parameter ID and may be cast to the appropriate
             data type (example: data type of uint16_t will be cast to
             uint16_t pointer).

    \return Success
*/
extern bool MemOpsService_SetParameter_UART(uint8_t param,
                                          uint16_t len,
                                          void *value);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif


#endif /* PROFILES_MEMOPS_SERVICE_H_ */
