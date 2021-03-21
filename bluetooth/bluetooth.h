/*! \file "bluetooth.h"
   \brief Bluetooth subsystem header file

    Contains global function declarations
*/

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "stm32f4xx_hal.h"

#include "rtos.h"
#include "peripherals.h"

#include "audio/audio.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS ------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\enum BTMode
	Bluetooth subsystem state

	BT_SLEEPMODE	Sleep mode
	BT_ACTIVEMODE	Active mode
	BT_AIRPLANEMODE	Airplane mode
 */
typedef enum
{
	BT_INITIALISED = 0,
	BT_SLEEPMODE,
	BT_ACTIVEMODE,
	BT_AIRPLANEMODE
} BTMode;

/*!\typedef BTState
	Contains all necessary state variables for Bluetooth subsystem


 */
typedef struct
{
	AudioState *audio;
	PeriObject *peri;

	GPIO_TypeDef *shutdown_port;
	uint16_t shutdown_pin;

	BTMode bt_mode;
	BTMode recovery_mode;

	bool module_enabled;
	bool ble_enabled;
	bool classic_enabled;
	bool audio_enabled;

	bool pairing;
	bool paired;

} BTState;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CONSTANTS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Initialise bluetooth variables

    \param *state Pointer to bluetooth state object

    \remark Function declaration shall evolve as feature written

    \remark TODO
*/
void btInit(BTState *state, AudioState *audio, PeriObject *peri,
		GPIO_TypeDef *shutdown_port, uint16_t shutdown_pin);

/*! \brief Bring Module in and out of shutdown mode

    \param *state Pointer to bluetooth state object
    \param enable Enable or disable module

    \remark TODO
*/
void btModuleControl(BTState *state, bool enable);

/*! \brief Enable and disable BLE on module

    \param *state Pointer to bluetooth state object
    \param enable Enable or disable BLE

    \remark TODO
*/
void btBLEControl(BTState *state, bool enable);

/*! \brief Enable and disable BT Classic on module

    \param *state Pointer to bluetooth state object
    \param enable Enable or disable BT Classic

    \remark TODO
*/
void btClassicControl(BTState *state, bool enable);

/*! \brief Enable and disable BT Audio on module

    \param *state Pointer to bluetooth state object
    \param enable Enable or disable BT Audio

    \remark TODO
*/
void btAudioControl(BTState *state, bool enable);

/*! \brief Enter and exit Airplane mode

    \param *state Pointer to bluetooth state object
    \param enable Enable or disable Airplane mode

    \remark TODO
*/
void btAirplaneMode(BTState *state, bool enable);

/*! \brief Start BTAudio pairing

    \param *state Pointer to bluetooth state object

    \remark TODO
*/
void btAudioPair(BTState *state);

/*! \brief Configure Bluetooth for System SLEEP mode

    \param *state Pointer to bluetooth state object

    \remark TODO
*/
void btSleepMode(BTState *state);

/*! \brief Configure Bluetooth for System ACTIVE mode

    \param *state Pointer to bluetooth state object

    \remark TODO
*/
void btActiveMode(BTState *state);

/*! \brief Check if BT Module enabled

    \param *state Pointer to bluetooth state object
    \return Is BT Module enabled?

    \remark TODO
*/
bool btModuleEnabled(BTState *state);

/*! \brief Check BLE status

    \param *state Pointer to bluetooth state object
    \return Is BLE enabled?

    \remark TODO
*/
bool btBLEEnabled(BTState *state);

/*! \brief Check BT Classic status

    \param *state Pointer to bluetooth state object
    \return Is BT Classic enabled?

    \remark TODO
*/
bool btClassicEnabled(BTState *state);

/*! \brief Check BT Audio status

    \param *state Pointer to bluetooth state object
    \return Is BT Audio enabled?

    \remark TODO
*/
bool btAudioEnabled(BTState *state);

/*! \brief Check if BT Audio pairing

    \param *state Pointer to bluetooth state object
    \return Is BT Audio pairing?

    \remark TODO
*/
bool btIsPairing(BTState *state);

/*! \brief Check if BT Audio paired

    \param *state Pointer to bluetooth state object
    \return Is BT Audio paired?

    \remark TODO
*/
bool btIsPaired(BTState *state);

/*! \brief Check if we are in Airplane mode

    \param *state Pointer to bluetooth state object
    \return Is subsystem in Airplane mode?

    \remark TODO
*/
bool btIsAirplaneMode(BTState *state);

/*! \brief BT Audio Send Play Pause command

    \param *state Pointer to bluetooth state object

    \remark TODO
*/
void btSendPlayPause(BTState *state);

#ifdef __cplusplus
}
#endif

#endif /* BLUETOOTH_H_ */
