/*! \file "powerui_pmic.h"
   \brief PowerUI PMIC subsystem file
*/

#ifndef POWERUI_PMIC_H_
#define POWERUI_PMIC_H_

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
/* PMIC CONSTANTS ------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\var uint16_t PWRUI_MEMCHUNK_PWRUI_IND
	Index of PMIC Memchunk on memory map
*/
static const uint16_t PWRUI_MEMCHUNK_PWRUI_IND = 5;

/*!\var uint16_t PWRUI_CFG_MEMELEM_INDEX
	Index of PMIC Configuration Element in PMIC Memchunk
*/
static const uint16_t PWRUI_CFG_MEMELEM_INDEX = 0;

/*!\var uint8_t BQ_I2C_ADDR
	BQ2512XA I2C address
*/
static const uint8_t BQ_I2C_ADDR = 0x6A; //0b01101010

/*!\var uint32_t PWR_STATUPD_PERIODMS
	Period between PMIC status updates
*/
static const uint32_t PWR_STATUPD_PERIODMS = 5000;

/*!\def BQ_CD_ACTIVE
	Written value of CD pin in Active mode
*/
#define BQ_CD_ACTIVE    1

/*!\def BQ_CD_CHGEN
	Written value of CD pin in Charge-Enabled mode
*/
#define BQ_CD_HIZ     0

/*!\def BQ_INT_CHARGING
	Value of INT when charging
*/
#define BQ_INT_CHARGING    0

/*!\def BQ_INT_NOTCHARGING
	Value of INT when not charging
*/
#define BQ_INT_NOTCHARGING     1

/*!\def BQ_LSCNTRL_EN
	Written value of LSCNTRL to enable LS/LDO output
*/
#define BQ_LSCNTRL_EN    1

/*!\def BQ_LSCNTRL_DIS
	Written value of LSCNTRL to disable LS/LDO output
*/
#define BQ_LSCNTRL_DIS     0

/*!\def BQ_1V2EN_EN
	Written value of LSCNTRL to enable LS/LDO output
*/
#define BQ_1V2EN_EN    1

/*!\def BQ_1V2EN_DIS
	Written value of LSCNTRL to disable LS/LDO output
*/
#define BQ_1V2EN_DIS     0

/*!\def VBAT_LS_EN
	Written value to LS_EN pin to enable loadswitch output
*/
#define VBAT_LS_EN    1

/*!\def VBAT_LS_DIS
	Written value to LS_EN pin to disable loadswitch output
*/
#define VBAT_LS_DIS     0

/*!\def BQ_CFGADDR_NUM
	Number of BQ2512X Registers
*/
#define BQ_CFGADDR_NUM  10

/*!\var uint16_t CD_SAFETYPERIOD_MS
	Amount of time in mS to wait writing a new GPIO value to PMIC
*/
static const uint16_t CD_SAFETYPERIOD_MS = 15;

//PMIC Register Addresses
#define BQ_REG_STATUS   0x00
#define BQ_REG_FAULT    0x01
#define BQ_REG_TS       0x02
#define BQ_REG_CHARGE   0x03
#define BQ_REG_TERM     0x04
#define BQ_REG_BATREG   0x05
#define BQ_REG_SYSVOUT  0x06
#define BQ_REG_LSLDO    0x07
#define BQ_REG_PBCTRL   0x08
#define BQ_REG_ILIM     0x09
#define BQ_REG_BATMON   0x0A
#define BQ_REG_VINDPM   0x0B

//Default PMIC Register values
#define BQ_DEF_Fault_Mask   0b00000000      //No Masks
#define BQ_DEF_TSFault_Mask 0b00001111      //TS Disabled, EN_INT enabled, Mask all interrupts
#define BQ_DEF_ICHRG        0b10010000      //40+40mA charge, Charge enabled, No Hi-Z
#define BQ_DEF_IPRETERM     0b00101010      //Termination enabled, 5mA or 0.025C
#define BQ_DEF_VBREG        0b01101010      //Set to 4.13V
#define BQ_DEF_SYSVOUT      0b10101010      //Enable SYSOUT at 1.8V
#define BQ_DEF_LSLDO        0b00101000      //Disable LSLDO, set at 1.8V
#define BQ_DEF_PBCTRL       0b01001000		//PG used to indicate charge, WAKE1 80us, Shipmode on Reset (9s MR)
#define BQ_DEF_ILIMUVLO     0b00111010      //INLIM 400mA, BUVLO 3.0V
#define BQ_DEF_VINDPM       0b00100010      //Vindpm enabled, 4.4V, Safety timer 3hrs

/*!\var BQConfigReg BQ_CONFIG_DEFAULTS
	Default PMIC register settings. Only used if PMIC Config in NVS empty
*/
static const BQConfigReg BQ_CONFIG_DEFAULTS[BQ_CFGADDR_NUM] = {
  {BQ_REG_FAULT, BQ_DEF_Fault_Mask},
  {BQ_REG_TS, BQ_DEF_TSFault_Mask},
  {BQ_REG_CHARGE, BQ_DEF_ICHRG},
  {BQ_REG_TERM, BQ_DEF_IPRETERM},
  {BQ_REG_BATREG, BQ_DEF_VBREG},
  {BQ_REG_SYSVOUT, BQ_DEF_SYSVOUT},
  {BQ_REG_LSLDO, BQ_DEF_LSLDO},
  {BQ_REG_PBCTRL, BQ_DEF_PBCTRL},
  {BQ_REG_ILIM, BQ_DEF_ILIMUVLO},
  {BQ_REG_VINDPM, BQ_DEF_VINDPM}
};

/*!\def BQ_SOC_ZEROVOLTAGE
	Percentage of VBREG (4.13V) set as zero SOC
	Set to 3.3V which corresponds to ~10% total SOC at 0.2C for Li-ion
*/
#define BQ_SOC_ZEROVOLTAGE	80

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* PMIC FUNCTIONS ------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Write to PMIC

    \param *state Pointer to PowerUIState object
    \param addr Address to write to
    \param data Data to write
    \return Pass or fail
*/
bool powerui_bq_write_address(PowerUIState *state, uint8_t addr, uint8_t data);

/*! \brief Read from PMIC

    \param *state Pointer to PowerUIState object
    \param addr Address to read from
    \param *data Pointer to read data to
    \return Pass or fail
*/
bool powerui_bq_read_address(PowerUIState *state, uint8_t addr, uint8_t *data);

/*! \brief Set PMIC Charge Enable

    \param *state Pointer to PowerUIState object
*/
void powerui_bq_ChipDisable(PowerUIState *state);

/*! \brief Set PMIC Charge Disable

    \param *state Pointer to PowerUIState object
*/
void powerui_bq_ChipEnable(PowerUIState *state);

/*! \brief Check if Chargine

    \param *state Pointer to PowerUIState object
    \return Charging or not
*/
bool powerui_bq_ChargingCheck(PowerUIState *state);

#ifdef __cplusplus
}
#endif

#endif /* POWERUI_PMIC_H_ */
