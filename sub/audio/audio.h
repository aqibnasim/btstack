/*! \file "audio.h"
   \brief Audio header file

    Include for access to global audio variables and functions
    Functions audio subsystem control and configuration
*/

#ifndef AUDIO_AUDIO_H_
#define AUDIO_AUDIO_H_

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

#include "rtos.h"
#include "peripherals.h"

#include "audio_mem.h"
#include "audio_types.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CODEC/DSP REGISTER CONSTANTS ----------------------------------------*/
/*----------------------------------------------------------------------*/

/*!\brief CS47L90 Software reset register address
*/
#define  CS_ADDR_SWRESET      0x0000

/*!\brief CS47L90 clock-setup registers
*/
#define  CS_ADDR_SYSCLK1      0x0101
#define  CS_ADDR_DSPCLK1      0x0120
#define  CS_ADDR_32KCLK1      0x0100

/*!\brief CS47L90 clock-setup values
*/
#define CS_SYSCLK_EN          0x0444
#define CS_SYSCLK_DIS         0x0404
#define CS_DSPCLK_EN          0x0444
#define CS_DSPCLK_DIS         0x0404
#define CS_32KCLK_EN          0x0042
#define CS_32KCLK_DIS         0x0002

/*!\brief CS47L90 DSP Core control register values
*/
static const uint32_t AUDIO_DSP_CONFIG1_ADDR        = 0xFFE00;
static const uint32_t AUDIO_DSP_STATUS3_FREQSTS     = 0xFFE08;
static const uint32_t AUDIO_DSP_CONFIG1_START       = 0x00000013;
static const uint32_t AUDIO_DSP_CONFIG1_COREENABLE  = 0x00000012;
static const uint32_t AUDIO_DSP_CONFIG1_MEMEN       = 0x00000010;

/*!\brief CS47L90 AIF control register values
*/
static const uint16_t AUDIO_AIF_JUMP	       		= 0x0040;
static const uint16_t AUDIO_AIF_RX_START       		= 0x051A;
static const uint16_t AUDIO_AIF_TX_START       		= 0x0519;
static const uint16_t AUDIO_AIF_ENABLE				= 0x0002;
static const uint16_t AUDIO_AIF_DISABLE				= 0x0000;
static const uint8_t AUDIO_AIF_CH_EXT				= 0;
static const uint8_t AUDIO_AIF_CH_UC				= 1;
static const uint8_t AUDIO_AIF_CH_BT				= 2;
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* GLOBAL FUNCTIONS ----------------------------------------------------*/
/*----------------------------------------------------------------------*/

/*! \brief Initialize Audio subsystem parameters/variables/peripherals

    \param *state Pointer to Audio object
    \param *pwr Pointer to PowerUI object
    \param *peri Pointer to Peripheral object
    \param *cs_port Pointer to port of SPI Chip select
    \param cs_pin Pin number of SPI Chip select
*/
void audioInit(AudioState *state, PowerUIState *pwr, PeriObject *peri,
		GPIO_TypeDef *cs_port, uint16_t cs_pin);

/*! \brief Load default values into NVM (values stored in FLASH)

    \param *state Pointer to Audio object
    \return Pass or fail
*/
bool audioLoadDefaultsToNVM(AudioState *state);

/*! \brief Initialize Audio subsystem

   Initialize CS47L90 in stages.
   1) Turn power on via PMIC control
   2) Load contents of AUD_CFG memchunk
   3) Load input/output config
   4) Load DSP config/programs/parameters as needed

   \remark Contents of AUD_CFG need to be compatible with this function. ie
    Elements at the front need to fulfill the role set out in the comments here.

   \deprecated LED functionality to be removed

    \param *state Pointer to Audio object
    \return Pass or fail
*/
bool audio_cs_init(AudioState *state);

/*! \brief Set Audio subsystem into sleep mode

    \param *state Pointer to Audio object
    \return Pass or fail

    \bug Usage of clock register value macros to disable clocks is incorrect.
   	   Read contents, set enable/disable bit, and write-back.
*/
bool audio_cs_slpenter(AudioState *state);

/*! \brief Check Audio subsystem status

    \param *state Pointer to Audio object
    \return Audio status
*/
CSStatus audio_status(AudioState *state);

/*! \brief Configure all CS47L90 input channels with settings stored in RAM

    \param *state Pointer to Audio object
    \return Pass or Fail
*/
bool audio_Input_SetAll(AudioState *state);

/*! \brief Set volume of a specific input channel on the CS47L90

    \param *state Pointer to Audio object
    \param channel Channel to configure
    \param volume Volume in dB(x10). First decibel only
    \return Pass or Fail
*/
bool audio_Input_VolumeSet(AudioState *state, uint8_t channel, int16_t volume);

/*! \brief Mute/unMute a specific input channel on the CS47L90

    \param *state Pointer to Audio object
    \param channel Channel to configure
    \param mute Mute or unmute (true or false respectively)
    \return Pass or Fail
*/
bool audio_Input_MuteSet(AudioState *state, uint8_t channel, bool mute);

/*! \brief Configure all CS47L90 output channels with settings stored in RAM

    \param *state Pointer to Audio object
    \return Pass or Fail
*/
bool audio_Output_SetAll(AudioState *state);

/*! \brief Mute/unMute a specific ouput channel on the CS47L90

    \param *state Pointer to Audio object
    \param channel Channel to configure
    \param volume Volume in dB(x10). First decibel only
    \return Pass or Fail
*/
bool audio_Output_VolumeSet(AudioState *state, uint8_t channel, int16_t volume);

/*! \brief Mute/unMute a specific output channel on the CS47L90

    \param *state Pointer to Audio object
    \param channel Channel to configure
    \param mute Mute or unmute (true or false respectively)
    \return Pass or Fail
*/
bool audio_Output_MuteSet(AudioState *state, uint8_t channel, bool mute);

/*! \brief Set EQ bands for specified channel

    \param *state Pointer to Audio object
    \param channel Channel to configure
    \param *bands Array of band gains
    \return Pass or Fail
*/
bool audio_EQ_Set(AudioState *state, uint8_t channel, int16_t *bands);

/*! \brief Play an audio clip stored at audfile_chunk->audfile_index. This
 	 function blocks with a pend to Semaphore at *sem.

    \param *state Pointer to AudioState object
    \param audfile_index Element index of audio file
    \param *sem Semaphore used to signal file played

    \remark TODO
*/
void audio_playfile(AudioState *state, uint16_t audfile_index, SemaphoreHandle_t *sem);

/*! \brief Enable and disable AIF channels

    \param *state Pointer to AudioState object
    \param channel AIF channel to affect
    \param enable Enable or disable channel?
*/
void audio_AIF_enable(AudioState *state, uint8_t channel, bool enable);

/*! \brief Enable and disable AIF channels

    \param *state Pointer to AudioState object
    \param channel AIF channel to affect
    \param enable Enable or disable channel?
*/
void audio_AIF_enable(AudioState *state, uint8_t channel, bool enable);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_AUDIO_H_ */

