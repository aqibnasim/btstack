/*! \file "powerui_led.h"
   \brief PowerUI LED subsystem file
*/

#ifndef POWERUI_LED_H_
#define POWERUI_LED_H_

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

#include "powerui_types.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* LED IC CONSTANTS ----------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\def PWRUI_LED_NUMANIM
	Number of LED animations
*/
#define PWRUI_LED_NUMANIM	3

/*!\var uint16_t PWRUI_MEMCHUNK_LED_IND
	Index of LED Memchunk on memory map
*/
static const uint16_t PWRUI_MEMCHUNK_LED_IND = 6;

/*!\var uint16_t PWRUI_LEDANIM_MEMELEM_INDEX
	Index of LED Animations
*/
static const uint16_t PWRUI_LEDANIM_MEMELEM_INDEX = 0;

/*!\var uint16_t PWRUI_LEDANIM_RETRIES
	Number of times to retry writing an LED sequence
*/
static const uint16_t PWRUI_LEDANIM_RETRIES = 3;

/*!\var uint8_t NCP_I2C_ADDR
	NCP5623B I2C address
*/
static const uint8_t NCP_I2C_ADDR = 0x38;

/*!\var uint8_t NCP_MAX_PWM
	Maximum value to write to PWM register
*/
static const uint8_t NCP_MAX_PWM = 0x1F;

/*!\var uint8_t NCP_MAX_STEPS
	Maximum value to write to Maxsteps register
*/
static const uint8_t NCP_MAX_STEPS = 0x1F;

/*!\var uint8_t NCP_MAX_LEDCH
	Red channel index
*/
static const uint8_t NCP_MAX_LEDCH = 3;

/*!\var uint8_t NCP_LEDCH_R
	Red channel index
*/
static const uint8_t NCP_LEDCH_R = 0;

/*!\var uint8_t NCP_LEDCH_B
	Blue channel index
*/
static const uint8_t NCP_LEDCH_B = 1;

/*!\var uint8_t NCP_LEDCH_W
	White channel index
*/
static const uint8_t NCP_LEDCH_W = 2;

//LED IC Register addresses
#define NCP_REG_CSHUTDOWN   0x00
#define NCP_REG_CURRSTEP    0x01
#define NCP_REG_LEDPWMBASE  0x02
#define NCP_REG_LEDREGJUMP  0x01
#define NCP_REG_IENDUP      0x05
#define NCP_REG_IENDDWN     0x06
#define NCP_REG_STEPTIME    0x07

static const uint8_t LED_ANIM_PAIR_IND = 0;
static const uint8_t LED_ANIM_CHARGE_IND = 1;
static const uint8_t LED_ANIM_LOWBAT_IND = 2;

/*!\var LEDAnim LED_ANIMATIONS
	Default Animation settings. Defaults used only if NVS is empty.
*/
static LEDAnim LED_ANIMATIONS[PWRUI_LED_NUMANIM] = {
		{
			LED_Anim_Pairing, 1, 1,
			0x01, 30, 1, 200, 200
		},
		{
			LED_Anim_Charging, 0, 1,
			0x08, 30, 1, 1000, 1000
		},
		{
			LED_Anim_Lowbattery, 0, 1,
			0x04, 15, 0, 20, 8000
		}
};

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Shutdown LED IC

    \param *state Pointer to PowerUIState object
    \return Pass or fail
*/
bool powerui_ncp_shutdown(PowerUIState *state);

/*! \brief Set LED IC Current step

    \param *state Pointer to PowerUIState object
    \param step Step level
    \return Pass or fail
*/
bool powerui_ncp_currentstep(PowerUIState *state, uint8_t step);

/*! \brief LED timer function

    \param xTimer Timer Handle
*/
void powerui_ledtimer_clockfn(TimerHandle_t xTimer);

/*! \brief Start an animation

    \param *state Pointer to PowerUIState object
    \param *anim Pointer to animation being played
    \return Pass or fail
*/
bool powerui_ncp_startanimation(PowerUIState *state, LEDAnim *anim);

#ifdef __cplusplus
}
#endif

#endif /* POWERUI_LED_H_ */
