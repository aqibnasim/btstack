/*! \file "devctrl_service.h"
   \brief Contains details for Service and Characteristics

   Function for Parameter setting
*/

#ifndef PROFILES_DEVCTRL_SERVICE_H_
#define PROFILES_DEVCTRL_SERVICE_H_

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
#define DEVCTRL_UUID_BASE128(uuid) 0xB3, 0xDE, 0xEB, 0x48, 0xA8, 0xDE, \
    0x81, 0x9C, 0xD4, 0x4B, 0xFE, 0xB7, \
    LO_UINT16(uuid), HI_UINT16(uuid), 0x47, 0xA8

/*!\def DEVCTRL_SERV_UUID
	4bit Service UUID
*/
#define DEVCTRL_SERV_UUID 0x0000

// CHARACTERISTIC Details
// dcDevVolumeSet
#define BTDEV_VOLSET_ID         0
#define BTDEV_VOLSET_UUID       0x0000
#define BTDEV_VOLSET_LEN        4
#define BTDEV_VOLSET_LEN_MIN    3

// dcDevDeviceIDSet
#define BTDEV_DEVIDSET_ID         1
#define BTDEV_DEVIDSET_UUID       0x0001
#define BTDEV_DEVIDSET_LEN        20
#define BTDEV_DEVIDSET_LEN_MIN    21

// dcDevDeviceCalibrati on1
#define BTDEV_DEVCALIB1_ID         2
#define BTDEV_DEVCALIB1_UUID       0x0002
#define BTDEV_DEVCALIB1_LEN        10
#define BTDEV_DEVCALIB1_LEN_MIN    9

// dcDevSleepControl
#define BTDEV_SLEEPCTRL_ID         3
#define BTDEV_SLEEPCTRL_UUID       0x0003
#define BTDEV_SLEEPCTRL_LEN        2
#define BTDEV_SLEEPCTRL_LEN_MIN    2

// dcAirplaneMode
#define BTDEV_AIRPLANE_ID         4
#define BTDEV_AIRPLANE_UUID       0x0004
#define BTDEV_AIRPLANE_LEN        2
#define BTDEV_AIRPLANE_LEN_MIN    1

// dcEQControl
#define BTDEV_EQCTRL_ID         5
#define BTDEV_EQCTRL_UUID       0x0005
#define BTDEV_EQCTRL_LEN        12
#define BTDEV_EQCTRL_LEN_MIN    2

// dcAIControl
#define BTDEV_AICTRL_ID         6
#define BTDEV_AICTRL_UUID       0x0006
#define BTDEV_AICTRL_LEN        2
#define BTDEV_AICTRL_LEN_MIN    2

// dcSpeechEnhanceControl
#define BTDEV_SPEECHCTRL_ID         7
#define BTDEV_SPEECHCTRL_UUID       0x0007
#define BTDEV_SPEECHCTRL_LEN        2
#define BTDEV_SPEECHCTRL_LEN_MIN    2

// dcSmartBatteryControl
#define BTDEV_SMARTBATTCTRL_ID         8
#define BTDEV_SMARTBATTCTRL_UUID       0x0008
#define BTDEV_SMARTBATTCTRL_LEN        2
#define BTDEV_SMARTBATTCTRL_LEN_MIN    2

// dcFactoryReset
#define BTDEV_FACTORYRST_ID         9
#define BTDEV_FACTORYRST_UUID       0x0009
#define BTDEV_FACTORYRST_LEN        2
#define BTDEV_FACTORYRST_LEN_MIN    1

// dcUsageStatistics
#define BTDEV_USAGESTAT_ID         10
#define BTDEV_USAGESTAT_UUID       0x000A
#define BTDEV_USAGESTAT_LEN        9
#define BTDEV_USAGESTAT_LEN_MIN    2

// dcCalibrationMode
#define BTDEV_CALIBMODE_ID         11
#define BTDEV_CALIBMODE_UUID       0x000B
#define BTDEV_CALIBMODE_LEN        2
#define BTDEV_CALIBMODE_LEN_MIN    2

// dcDeviceInfo
#define BTDEV_DEVINFO_ID         12
#define BTDEV_DEVINFO_UUID       0x000C
#define BTDEV_DEVINFO_LEN        21
#define BTDEV_DEVINFO_LEN_MIN    1

// dcEQPoll
#define BTDEV_EQPOLL_ID         13
#define BTDEV_EQPOLL_UUID       0x000D
#define BTDEV_EQPOLL_LEN        13
#define BTDEV_EQPOLL_LEN_MIN    2

// dcVolumePoll
#define BTDEV_VOLPOLL_ID         14
#define BTDEV_VOLPOLL_UUID       0x000E
#define BTDEV_VOLPOLL_LEN        4
#define BTDEV_VOLPOLL_LEN_MIN    2

// dcSetPairing
#define BTDEV_SETPAIR_ID         15
#define BTDEV_SETPAIR_UUID       0x000F
#define BTDEV_SETPAIR_LEN        2
#define BTDEV_SETPAIR_LEN_MIN    1

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* API FUNCTIONS -------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Set a DevCtrlService parameter over BLE

    \param param Profile parameter ID
    \param len length of data to write
    \param *value pointer to data to write.  This is dependent on
             the parameter ID and may be cast to the appropriate
             data type (example: data type of uint16_t will be cast to
             uint16_t pointer).

    \return Success
*/
extern bool DevCtrlService_SetParameter_BLE(uint8_t param,
                                          uint16_t len,
                                          void *value);

/*! \brief Set a DevCtrlService parameter over UART

    \param param Profile parameter ID
    \param len length of data to write
    \param *value pointer to data to write.  This is dependent on
             the parameter ID and may be cast to the appropriate
             data type (example: data type of uint16_t will be cast to
             uint16_t pointer).

    \return Success
*/
extern bool DevCtrlService_SetParameter_UART(uint8_t param,
                                          uint16_t len,
                                          void *value);

/*----------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif


#endif /* PROFILES_DEVCTRL_SERVICE_H_ */
