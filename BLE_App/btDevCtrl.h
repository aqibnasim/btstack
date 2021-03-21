/*! \file "btDevCtrl.h"
   \brief Contains handler functions for all characteristics in
   "Device Control" service

   This service is used to execute device processes.
*/

#ifndef APPLICATION_BTDEVCTRL_H_
#define APPLICATION_BTDEVCTRL_H_

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include "devctrl_service.h"

//Subsystems
#include "audio/audio.h"
#include "powerui/powerui.h"
//#include "bluetooth.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CONSTANTS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\var uint8_t DEVCTRL_MSGTYPES_VOLSET
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_VOLSET = 2;

/*!\var uint8_t DEVCTRL_MSGTYPES_DEVIDSET
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_DEVIDSET = 2;

/*!\var uint8_t DEVCTRL_MSGTYPES_DEVCALIB1
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_DEVCALIB1 = 2;

/*!\var uint8_t DEVCTRL_MSGTYPES_SLEEPCTRL
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_SLEEPCTRL = 2;

/*!\var uint8_t DEVCTRL_MSGTYPES_AIRPLANE
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_AIRPLANE = 2;

/*!\var uint8_t DEVCTRL_MSGTYPES_EQCTRL
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_EQCTRL = 3;

/*!\var uint8_t DEVCTRL_MSGTYPES_AICTRL
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_AICTRL = 2;

/*!\var uint8_t DEVCTRL_MSGTYPES_SPCHCTRL
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_SPCHCTRL = 2;

/*!\var uint8_t DEVCTRL_MSGTYPES_SMARTBATCTRL
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_SMARTBATCTRL = 2;

/*!\var uint8_t DEVCTRL_MSGTYPES_FACTORYRESET
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_FACTORYRESET = 2;

/*!\var uint8_t DEVCTRL_MSGTYPES_USAGESTAT
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_USAGESTAT = 2;

/*!\var uint8_t DEVCTRL_MSGTYPES_CALIBMODE
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_CALIBMODE = 4;

/*!\var uint8_t DEVCTRL_MSGTYPES_DEVINFO
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_DEVINFO = 3;

/*!\var uint8_t DEVCTRL_MSGTYPES_EQPOLL
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_EQPOLL = 3;

/*!\var uint8_t DEVCTRL_MSGTYPES_VOLPOLL
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_VOLPOLL = 2;

/*!\var uint8_t DEVCTRL_MSGTYPES_SETPAIR
	Number of Message-types in Characteristic
*/
static const uint8_t DEVCTRL_MSGTYPES_SETPAIR = 2;

static const uint8_t DEVCTRL_NOPROCESS = 0xFF;

/*!\var uint8_t DEVCTRL_ALL_ACKERR_POS
	Index of Acknowledgement/Error Byte in incoming message
*/
static const uint8_t DEVCTRL_ALL_ACKERR_POS = 1;

/*!\var uint8_t DEVCTRL_PDU_SIZE
	Maximum size of Service data packet
*/
static const uint8_t DEVCTRL_PDU_SIZE = 16;                      //Odd numbers seem to be problematic

//VolumeSet
static const uint8_t DEVCTRL_VOLSET_1_LEN = 3;
static const uint8_t DEVCTRL_VOLSET_2_LEN = 4;

static const uint8_t DEVCTRL_VOLSET_1VOL_POS = 1;
static const uint8_t DEVCTRL_VOLSET_1CH_POS = 2;
static const uint8_t DEVCTRL_VOLSET_2VOL_POS = 2;
static const uint8_t DEVCTRL_VOLSET_2CH_POS = 3;

//DeviceIDSet
static const uint8_t DEVCTRL_DEVIDSET_1_LEN = 7;
static const uint8_t DEVCTRL_DEVIDSET_2_LEN = 8;

static const uint8_t DEVCTRL_DEVIDSET_1DEVID_POS = 1;
static const uint8_t DEVCTRL_DEVIDSET_1DEVIDASCII_POS = 3;
static const uint8_t DEVCTRL_DEVIDSET_2DEVID_POS = 2;
static const uint8_t DEVCTRL_DEVIDSET_2DEVIDASCII_POS = 4;

//DeviceCalibration1
static const uint8_t DEVCTRL_DEVCALIB1_1_LEN = 9;
static const uint8_t DEVCTRL_DEVCALIB1_2_LEN = 10;

static const uint8_t DEVCTRL_DEVCALIB1_1LOUTADJ_POS = 1;
static const uint8_t DEVCTRL_DEVCALIB1_1ROUTADJ_POS = 3;
static const uint8_t DEVCTRL_DEVCALIB1_1LINADJ_POS = 5;
static const uint8_t DEVCTRL_DEVCALIB1_1RINADJ_POS = 7;
static const uint8_t DEVCTRL_DEVCALIB1_2LOUTADJ_POS = 2;
static const uint8_t DEVCTRL_DEVCALIB1_2ROUTADJ_POS = 4;
static const uint8_t DEVCTRL_DEVCALIB1_2LINADJ_POS = 6;
static const uint8_t DEVCTRL_DEVCALIB1_2RINADJ_POS = 8;

//SleepControl
static const uint8_t DEVCTRL_SLEEPCTRL_1_LEN = 2;
static const uint8_t DEVCTRL_SLEEPCTRL_2_LEN = 2;

static const uint8_t DEVCTRL_SLEEPCTRL_1ONOFF_POS = 1;

//AirplaneMode
static const uint8_t DEVCTRL_AIRPLANE_1_LEN = 1;
static const uint8_t DEVCTRL_AIRPLANE_2_LEN = 2;

//EQControl
static const uint8_t DEVCTRL_EQCTRL_1_LEN = 12;
static const uint8_t DEVCTRL_EQCTRL_2_LEN = 2;

static const uint8_t DEVCTRL_EQCTRL_1CH_POS = 1;
static const uint8_t DEVCTRL_EQCTRL_1BAND1_POS = 2;
static const uint8_t DEVCTRL_EQCTRL_1BAND2_POS = 4;
static const uint8_t DEVCTRL_EQCTRL_1BAND3_POS = 6;
static const uint8_t DEVCTRL_EQCTRL_1BAND4_POS = 8;
static const uint8_t DEVCTRL_EQCTRL_1BAND5_POS = 10;

//AIControl
static const uint8_t DEVCTRL_AICTRL_1_LEN = 2;
static const uint8_t DEVCTRL_AICTRL_2_LEN = 2;

static const uint8_t DEVCTRL_AICTRL_1ONOFF_POS = 1;

//SPCHControl
static const uint8_t DEVCTRL_SPCHCTRL_1_LEN = 2;
static const uint8_t DEVCTRL_SPCHCTRL_2_LEN = 2;

static const uint8_t DEVCTRL_SPCHCTRL_1ONOFF_POS = 1;

//SMARTBATControl
static const uint8_t DEVCTRL_SMARTBATCTRL_1_LEN = 2;
static const uint8_t DEVCTRL_SMARTBATCTRL_2_LEN = 2;

static const uint8_t DEVCTRL_SMARTBATCTRL_1ONOFF_POS = 1;

//FACTORYRESET
static const uint8_t DEVCTRL_FACTORYRESET_1_LEN = 1;
static const uint8_t DEVCTRL_FACTORYRESET_2_LEN = 2;

//USAGESTAT
static const uint8_t DEVCTRL_USAGESTAT_1_LEN = 9;
static const uint8_t DEVCTRL_USAGESTAT_2_LEN = 2;

static const uint8_t DEVCTRL_USAGESTAT_1MINTODAY_POS = 1;
static const uint8_t DEVCTRL_USAGESTAT_1MINYEST_POS = 3;

//CALIBMODE
static const uint8_t DEVCTRL_CALIBMODE_1_LEN = 2;
static const uint8_t DEVCTRL_CALIBMODE_2_LEN = 2;
static const uint8_t DEVCTRL_CALIBMODE_3_LEN = 2;
static const uint8_t DEVCTRL_CALIBMODE_4_LEN = 2;

static const uint8_t DEVCTRL_CALIBMODE_1TONE_POS = 1;
static const uint8_t DEVCTRL_CALIBMODE_3VOL_POS = 1;

//DEVINFO
static const uint8_t DEVCTRL_DEVINFO_1_LEN = 1;
static const uint8_t DEVCTRL_DEVINFO_2_LEN = 15;

static const uint8_t DEVCTRL_DEVINFO_1DEVID_POS = 2;
static const uint8_t DEVCTRL_DEVINFO_1DEVNAME_POS = 6;
static const uint8_t DEVCTRL_DEVINFO_1CAL_POS = 13;
static const uint8_t DEVCTRL_DEVINFO_1CHRGSTAT_POS = 14;

//EQPOLL
static const uint8_t DEVCTRL_EQPOLL_1_LEN = 2;
static const uint8_t DEVCTRL_EQPOLL_2_LEN = 13;

static const uint8_t DEVCTRL_EQPOLL_1CH_POS = 1;
static const uint8_t DEVCTRL_EQPOLL_2CH_POS = 2;
static const uint8_t DEVCTRL_EQPOLL_2BAND1_POS = 3;
static const uint8_t DEVCTRL_EQPOLL_2BAND2_POS = 5;
static const uint8_t DEVCTRL_EQPOLL_2BAND3_POS = 7;
static const uint8_t DEVCTRL_EQPOLL_2BAND4_POS = 9;
static const uint8_t DEVCTRL_EQPOLL_2BAND5_POS = 11;

//VolumePoll
static const uint8_t DEVCTRL_VOLPOLL_1_LEN = 2;
static const uint8_t DEVCTRL_VOLPOLL_2_LEN = 4;

static const uint8_t DEVCTRL_VOLPOLL_1CH_POS = 1;
static const uint8_t DEVCTRL_VOLPOLL_2VOL_POS = 2;
static const uint8_t DEVCTRL_VOLPOLL_2CH_POS = 3;

//SetPairing
static const uint8_t DEVCTRL_SETPAIR_1_LEN = 1;
static const uint8_t DEVCTRL_SETPAIR_2_LEN = 2;


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS ------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\enum btDevCtrlInterface
 Interface used in communication
*/
typedef enum
{
	DEVCTRL_UART,
	DEVCTRL_BLE,
} btDevCtrlInterface;

/*!\enum btDevCtrlStatus
 List of function status returns
*/
typedef enum
{
    DEVCTRL_STAT_SUCCESS = 0,
    DEVCTRL_STAT_FAIL,
    DEVCTRL_STAT_INVALIDTYPE,
    DEVCTRL_STAT_INVALIDPARAM,
    DEVCTRL_STAT_PROCESSBUSY
} btDevCtrlStatus;

/*!\enum btDevCtrlResponses
 List of responses to send back to Host
*/
typedef enum
{
    DEVCTRL_RESP_SUCCESS = 0,
    DEVCTRL_RESP_IDINVALID,
    DEVCTRL_RESP_VALUEINVALID,
    DEVCTRL_RESP_CHANNELINVALID,
    DEVCTRL_RESP_PARAMETERINVALID,
	DEVCTRL_RESP_PROCESSFAILED,
} btDevCtrlResponses;

/*!\typedef btDevCtrlState
 Contains the state of the Service. Used to store data during
 multi-message sequential communication.

 \var btDevCtrlInterface btDevCtrlState::interface
 Interface used in communication
*/
typedef struct
{
    btDevCtrlInterface interface;

    int16_t eq_bands[10];
} btDevCtrlState;

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
btDevCtrlStatus btDevCtrlService_Handler(btDevCtrlInterface interface, mainState *state, uint8_t *pCharData, uint8_t charId);


#endif /* APPLICATION_BTDEVCTRL_H_ */
