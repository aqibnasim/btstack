/*! \file "statemachine.h"
   \brief State Machine header file

    Contains the System state machine task function
    Definition for mainState object
*/

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

//Subsystems
#include "audio/audio.h"
#include "powerui/powerui.h"
//#include "bluetooth.h"

#include "statemachine_types.h"

#include "main.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* MAIN OBJECT ---------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\var mainState main_state
	MainState Object. All system data stored here.
*/
mainState main_state;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CONSTANTS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\var uint16_t SM_LOOP_PERIODMS
	Amount of time between loop executions
*/
static const uint16_t SM_LOOP_PERIODMS = 200;

/*!\var uint16_t SM_LOWPOWER_PERCENT
	Battery Percent below which we are considered low power
*/
static const int8_t SM_LOWPOWER_PERCENT = 0;

/*!\var uint16_t SM_BATLOW_PERCENT
	Battery Percent below which we are considered battery low
*/
static const int8_t SM_BATLOW_PERCENT = 20;

/*!\var uint16_t SM_BATCRITICAL_PERCENT
	Battery Percent below which we are considered battery critical
*/
static const int8_t SM_BATCRITICAL_PERCENT = 10;

/*!\var uint8_t SM_MEMCHUNK_FACTORYRST_IND
	Index of Factory Reset chunk in memory map
*/
static const uint8_t SM_MEMCHUNK_FACTORYRST_IND = 7;

/*!\var uint8_t SM_MEMCHUNK_SYSDETAIL_IND
	Index of System detail chunk in memory map
*/
static const uint8_t SM_MEMCHUNK_SYSDETAIL_IND = 8;

/*!\var uint8_t SM_FACTORYRST_SYSDETAIL_ELEMIND
	Index of System detail backup in Factory Backup chunk
*/
static const uint8_t SM_FACTORYRST_SYSDETAIL_ELEMIND = 0;

/*!\var uint8_t SM_FACTORYRST_INVOL_ELEMIND
	Index of Input configuration backup in Factory Backup chunk
*/
static const uint8_t SM_FACTORYRST_INVOL_ELEMIND = 1;

/*!\var uint8_t SM_FACTORYRST_INVOLADJ_ELEMIND
	Index of Input adjustment configuration backup in Factory Backup chunk
*/
static const uint8_t SM_FACTORYRST_INVOLADJ_ELEMIND = 2;

/*!\var uint8_t SM_FACTORYRST_OUTVOL_ELEMIND
	Index of Input configuration backup in Factory Backup chunk
*/
static const uint8_t SM_FACTORYRST_OUTVOL_ELEMIND = 3;

/*!\var uint8_t SM_FACTORYRST_OUTVOLADJ_ELEMIND
	Index of Input adjustment configuration backup in Factory Backup chunk
*/
static const uint8_t SM_FACTORYRST_OUTVOLADJ_ELEMIND = 4;

/*!\var uint8_t SM_FACTORYRST_EQ_ELEMIND
	Index of Input adjustment configuration backup in Factory Backup chunk
*/
static const uint8_t SM_FACTORYRST_EQ_ELEMIND = 5;

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief State machine task function

    \param *arg Pointer to mainState object
*/
void stateMachineTask(void *arg);

/*! \brief Queue a Soft Reset on next iteration of loop

    \param *state Pointer to mainState object
*/
void smQueueSoftReset(mainState *state);

/*! \brief Queue a Sleep on next iteration of loop

    \param *state Pointer to mainState object
*/
void smQueueSleep(mainState *state);

/*! \brief Queue a Wake on next iteration of loop

    \param *state Pointer to mainState object
*/
void smQueueWake(mainState *state);

/*! \brief Run a factory reset of the device

    \param *state Pointer to mainState object
*/
bool smFactoryReset(mainState *state);

/*! \brief Sleep entry function. Function called in tasks.c (in portTASK_FUNCTION).

    \param xExpectedIdleTime	Ticks to sleep
*/
void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime );


#ifdef __cplusplus
}
#endif

#endif /* STATEMACHINE_H_ */

