#ifndef STATEMACHINE_TYPES_H_
#define STATEMACHINE_TYPES_H_

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS ------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*!\typedef sysDetails
	Contains System Details.

 \var char mainState::devID
 Device ID
 \var char mainState::devName
 Device Name
 \var char mainState::model
 Model number
 */
typedef struct
{
	//Device Info
	char devID[4];
	char devName[13];
	char model[2];
	uint8_t fw_version;
	bool calibrated;
} sysDetails;


/*!\enum sysState
	Memory operation statuses that may be returned by a memory function

	STATE_STARTUP
    STATE_LOWPOWER
    STATE_ENTERLOWPOWER
 	STATE_SLEEP
 	STATE_ENTERSLEEP
 	STATE_ACTIVE
 	STATE_ENTERACTIVE
*/
typedef enum
{
	STATE_STARTUP,
	STATE_LOWPOWER,
	STATE_ENTERLOWPOWER,
	STATE_SLEEP,
	STATE_ENTERSLEEP,
	STATE_ACTIVE,
	STATE_ENTERACTIVE,
} sysState;

/*!\enum lpMode
	Low Power modes used by statemachine

	LP_SLEEP
    LP_STOP
*/
typedef enum
{
	LP_SLEEP,
	LP_STOP,
} lpMode;

/*!\enum audpClip
	List of audio clips to play

	AUDP_STARTUP: Played on startup of device (when frames opened)
	AUDP_BTON: Played when bluetooth audio turned on
	AUDP_BTOFF: Played when bluetooth audio turned off
	AUDP_BTPAIR: Played when entering pairing mode
	AUDP_BTPAIRED: Played when paired
	AUDP_FRCONFIRM: Played when factory reset requested
	AUDP_FR: Played when factory reset activated
	AUDP_BATLOW: Played battery low (20%)
	AUDP_BATCRITICAL: Played when battery critical (10%)
*/
typedef enum
{
	AUDP_STARTUP = 0,
	AUDP_BTON,
	AUDP_BTOFF,
	AUDP_BTPAIR,
	AUDP_BTPAIRED,
	AUDP_FRCONFIRM,
	AUDP_FR,
	AUDP_BATLOW,
	AUDP_BATCRITICAL,
} audpClip;

/*!\typedef mainState
	Contains all subsystem objects. Basically a superset shell for all
	other objects.

 \var TaskHandle_t mainState::sm_handle
 Task Handle for statemachine
 \var TaskHandle_t mainState::btapp_handle
 Task Handle for Bluetooth application
 \var char mainState::devID
 Device ID in ASCII
 \var char mainState::devName
 Device Name in ASCII
 \var bool mainState::soft_reset
 Set to trigger a soft reset.
 \var bool mainState::sleep_cmd
 Set to trigger system to enter sleep mode.
 \var bool mainState::wake_cmd
 Set to trigger system to enter active mode.
 \var SemaphoreHandle_t mainState::semhnd_cmd
 Semaphore for command management
 \var PeriObject mainState::peripherals
 Peripheral object
 \var PowerUIState mainState::power
 PowerUI subsystem object
 \var AudioState mainState::audio
 Audio subsystem object
 \var BTState mainState::bluetooth
 Bluetooth subsystem object
 \var sysState mainState::current_state
 Current system state
 \var sysState mainState::recovery_state
 Recovery system state
 \var lpMode mainState::lowpower_mode
 Low power mode used
 \var PBStatus mainState::pb_laststate
 Last pushbutton state
 \var bool mainState::usernotification_enabled
 User notifications enabled/disabled
 \var bool mainState::audplayed_batcritical
 Audio notification played flag
 \var bool mainState::audplayed_batlow
 Audio notification played flag
 \var bool mainState::audplayed_paired
 Audio notification played flag

*/
typedef struct
{
    TaskHandle_t sm_handle;
    TaskHandle_t btapp_handle;

    sysDetails details;

    //Commands
    bool soft_reset;
    bool sleep_cmd;
    bool wake_cmd;

    //Command management
    SemaphoreHandle_t semhnd_cmd;

    //Peripherals
    PeriObject peripherals;

    //Subsytems
    PowerUIState power;
    AudioState audio;
    BTState bluetooth;

    //State management
    sysState current_state;
    sysState recovery_state;
    lpMode lowpower_mode;

    //PB management
    PBStatus pb_laststate;

    //User notification
    bool usernotification_enabled;
    bool audplayed_batcritical;
    bool audplayed_batlow;
    bool audplayed_paired;


} mainState;


#endif /* STATEMACHINE_TYPES_H_ */
