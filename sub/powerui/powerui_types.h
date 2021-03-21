/*! \file "powerui_types.h"
   \brief PowerUI types subsystem file

   All types for PowerUI defined here
*/

#ifndef POWERUI_TYPES_H_
#define POWERUI_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS (PMIC) -----------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\enum BQStatus
 PMIC status
*/
typedef enum
{
    BQ_Ready = 0,
    BQ_Charging,
    BQ_Charged,
    BQ_Fault
} BQStatus;

/*!\typedef BQConfigReg
 PMIC Register/data object

 \var uint8_t BQConfigReg::addr
 BQ2512X Register address
 \var uint8_t BQConfigReg::data
 BQ2512X Register data
*/
typedef struct
{
    uint8_t addr;
    uint8_t data;
} BQConfigReg;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS (FRAME) ----------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\enum FrameStatus
 Temple state (opened, closed)
*/
typedef enum
{
    Frame_Closed = 0,
    Frame_Opened
} FrameStatus;


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS (PB) -------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\enum PBStatus
 Temple state (opened, closed)
*/
typedef enum
{
    PB_NoClick = 0,
    PB_SingleClick,
	PB_LongPress_3s,
	PB_LongPress_6s,
	PB_LongPress_10s,
	PB_LongPress_20s,
} PBStatus;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS (LED) ------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\enum LEDAnimation
 LED Animation currently being played
*/
typedef enum
{
	LED_Anim_None,
	LED_Anim_Pairing,
	LED_Anim_Charging,
	LED_Anim_Lowbattery,
} LEDAnimation;

/*!\enum LEDAnimState
 State of LED Animation currently being played
*/
typedef enum
{
	LED_Animstate_upward,
	LED_Animstate_downward,
} LEDAnimState;

/*!\typedef LEDAnim
 LED State

\var LEDAnimation LEDAnim::type
 Type of animation as listed in LEDAnimation enum
 \var uint8_t LEDAnim::channel
 Channel animation is played on
 \var uint8_t LEDAnim::pwm
 PWM setting of channel
 \var uint8_t LEDAnim::step_time
 Step time of up/down dimming
 \var uint8_t LEDAnim::top_iend
 Sets the topend current of LED. See datasheet for details
 \var uint8_t LEDAnim::bot_iend
 Sets the bottom current of LED. See datasheet for details
 \var uint16_t LEDAnim::tophold_period
 Time in ms to hold topend current of animation
 \var uint16_t LEDAnim::offhold_period
 Time in ms to hold bottom current of animation
*/
typedef struct
{
	LEDAnimation type;
	uint8_t channel;
	uint8_t pwm;
	uint8_t step_time;
	uint8_t top_iend;
	uint8_t bot_iend;
	uint16_t tophold_period;
	uint16_t bothold_period;
} LEDAnim;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* VARIABLE DEFINITIONS (POWERUI) --------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\enum PowerUICmdType
 List of PWRUI commands

 \remark to be modified for new additions
*/
typedef enum
{
    PWRUI_CMD_INIT = 0,
    PWR_CMD_LSLDO_EN,
    PWR_CMD_LSLDO_DIS,
    PWR_CMD_LS_EN,
    PWR_CMD_LS_DIS,
	PWR_CMD_ENTERSHIP,
    LED_CMD_OFF,
	LED_CMD_PAIR_START,
	LED_CMD_CHARGE_START,
	LED_CMD_LOWBAT_START,
} PowerUICmdType;

/*!\typedef BQFault
 BQ2512X Faults. To be updated at set intervals

 \var bool BQFault::bat_ocp
 Battery Overcurrent
 \var bool BQFault::bat_uvlo
 Battery Undervoltage
 \var bool BQFault::vin_uv
 USB UnderVoltage
 \var bool BQFault::vin_ov
 USB OverVoltage
 \var bool BQFault::ilim
 Output Current limit reached
*/
typedef struct
{
    bool bat_ocp;
    bool bat_uvlo;
    bool vin_uv;
    bool vin_ov;
    bool ilim;
} BQFault;

/*!\typedef PowerUICmd
 PWRUI Command Object

 \var PowerUICmdType PowerUICmd::cmd
 Command type found here
 \var MBCmdItems PowerUICmd::items
 Mailbox settings
*/
typedef struct
{
    PowerUICmdType cmd;
    MBCmdItems items;
} PowerUICmd;

/*!\typedef PowerUIState
 PWRUI Object. Contains all necessary data for PWRUI subsystem.

 \var TaskHandle_t PowerUIState::task_cmd
 Task handle for PWRUI command-handle thread
 \var QueueHandle_t PowerUIState::mail_cmd
 Command Queue Handle
 \var bool PowerUIState::status_update
 Time for a status update?
 \var TimerHandle_t PowerUIState::clock_stat
 Status update timer

 \var uint8_t PowerUIState::LDO_val
 LDO register value. Read on startup

 \var BQStatus PowerUIState::status
 Status of PMIC
 \var uint8_t PowerUIState::soc
 State of charge. Updated at intervals
 \var BQFault PowerUIState::fault
 PMIC faults

 \var bool PowerUIState::i2c_fail
 I2C fail flag
 \var bool PowerUIState::initialized
 PWRUI subsystem initialized?

 \var TimerHandle_t PowerUIState::led_timer
 LED Animation timer
 \var LEDAnimation PowerUIState::led_active_animation
 Currently active LED animation
 \var LEDAnimState PowerUIState::anim_state
 State of current animations
 \var LEDAnim* PowerUIState::anim_active
 Pointer to active animation
 \var LEDAnim PowerUIState::animations
 Stored animations

 \var TimerHandle_t PowerUIState::frame_debounce_clock
 Debounce timer for frame input
 \var FrameStatus PowerUIState::frame_status
 Frame opened/closed?
 \var FrameStatus PowerUIState::frame_status_last
 Last frame state (used to track changes)
 \var GPIO_TypeDef PowerUIState::*hall_port
 Pointer to port of Hall sensor
 \var uint16_t PowerUIState::hall_pin
 Hall sensor pin

 \var TimerHandle_t PowerUIState::pbPressEnable_clock
 Debounce timer for PB input
 \var TimerHandle_t PowerUIState::pbPressTimer_clock
 Used to time the length of each press
 \var PBStatus PowerUIState::pb_status
 Tracks state of Pushbutton
 \var GPIO_TypeDef PowerUIState::*pb_port
 Pointer to port of Pushbutton
 \var uint16_t PowerUIState::pb_pin
 Pushbutton pin

 \var PeriObject PowerUIState::*peri
 Pointer to peripheral object
 \var I2cTransaction PowerUIState::pwr_i2c_trans
 PMIC I2C Transaction settings. Initialized on startup
 \var I2cTransaction PowerUIState::led_i2c_trans
 LED I2C Transaction settings. Initialized on startup
 \var GPIO_TypeDef PowerUIState::*pmic_port
 Pointer to PMIC/LS IO Port
 \var uint16_t PowerUIState::cd_pin
 CD Pin Index
 \var uint16_t PowerUIState::lscntrl_pin
 LSCNTRL Pin Index
 \var uint16_t PowerUIState::int_pin
 PM_INT Pin Index
 \var uint16_t PowerUIState::rst_pin
 PM_RST Pin Index
 \var uint16_t PowerUIState::lsen_pin
 LS_EN Pin Index

 \remark To be updated for new pinouts
*/
typedef struct
{
    //RTOS: Command management
    TaskHandle_t task_cmd;
    QueueHandle_t mail_cmd;

    //LSLDO
    uint8_t LDO_val;

    //Status
    BQStatus status;
    int8_t soc;
    BQFault fault;
    //Status flags
    bool i2c_fail;
    bool initialized;

    //LED
    TimerHandle_t led_timer;
    LEDAnimation led_active_animation;
    LEDAnimState anim_state;
    LEDAnim *anim_active;
    LEDAnim *animations;

    //Hall sensor
    TimerHandle_t frame_debounce_clock;
    FrameStatus frame_status;
    FrameStatus frame_status_last;
    GPIO_TypeDef *hall_port;
    uint16_t hall_pin;

    //Pushbutton
    TimerHandle_t pbPressEnable_clock;
    TimerHandle_t pbPressTimer_clock;
    PBStatus pb_status;
    GPIO_TypeDef *pb_port;
    uint16_t pb_pin;

    //Communication bus
    PeriObject *peri;
    I2cTransaction pwr_i2c_trans;
    I2cTransaction led_i2c_trans;
    GPIO_TypeDef *pmic_port;
    uint16_t cd_pin;
	uint16_t lscntrl_pin;
	uint16_t int_pin;
	uint16_t ldo1v2en_pin;
	uint16_t lsen_pin;
} PowerUIState;



#endif /* POWERUI_TYPES_H_ */
