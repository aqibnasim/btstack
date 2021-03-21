/*! \file "audio_config.c"
   \brief Audio configuration

    All contents in this file are for debug and testing only.
    None of the contents are required for production unit, assuming BLE service
    "memops" is functional, and/or there are functional settings programmed onto NVS
*/


#include "audio.h"


#ifdef AUD_LOAD_CFG

/////////////////////////////////////////////////////////////////////////////////////
//Chunk 0: Audio cfg
//Clock addresses
#define  CS_ADDR_SYSCLK1      0x0101
#define  CS_ADDR_DSPCLK1      0x0120
#define  CS_ADDR_32KCLK1      0x0100
#define CS_SYSCLK_EN          0x0444
#define CS_SYSCLK_DIS         0x0404
#define CS_DSPCLK_EN          0x0444
#define CS_DSPCLK_DIS         0x0404
#define CS_32KCLK_EN          0x0042
#define CS_32KCLK_DIS         0x0002

/****************************************************/
// RESET
/****************************************************/
#define CS_ADDR_SWRST       0x0000
#define CS_SWRST_VAL        0x1234
#define AUDIO_CFG_RST_SZ                  4
static const AudMemReg AUDIO_CFG_RST[AUDIO_CFG_RST_SZ] = {
    //SW RESET
    {CS_ADDR_SWRST, CS_SWRST_VAL},
    //Test key
    { 0x008A,  0x5555 },
    { 0x008A,  0x5555 }, { 0x008A,  0xAAAA },
};

/****************************************************/
// INITIALIZATION
/****************************************************/
#define AUDIO_CFG_INIT_SZ                   29
static const AudMemReg AUDIO_CFG_INIT[AUDIO_CFG_INIT_SZ] = {
    //DAC IF Adjustment
    {0x04CF,  0x0730},
    //ADC VCO calibration
    {0x03A2,  0x2438},
    //DRE Gain Shift Threshold Default Update
    {0x0444,  0x8000},    {0x0443,  0x1CFA},
    //DRE Update
    {0x0441,  0xC750},
    //Disable test key
    {0x008A,  0xCCCC},    {0x008A,  0x3333},
    //AIF1 GPIO
    {0x171C,  0x2000},    {0x171E,  0x2000},    {0x1720,  0x2000},    {0x1722,  0x2000},
    //AIF2 GPIO
    {0x1724,  0x2000},    {0x1726,  0x2000},    {0x1728,  0x2000},    {0x172A,  0x2000},
    //AIF3 GPIO
    {0x172C,  0x2000},    {0x172E,  0x2000},    {0x1730,  0x2000},    {0x1732,  0x2000},
    //DMIC GPIO
    {0x1744,  0x2000},    {0x1745,  0xF000},    {0x1746,  0x2000},    {0x1747,  0xF000},
    //Output GPIO (4)
    {0x1748,  0x2000},    {0x1749,  0xF000},    {0x174A,  0x2000},    {0x174B,  0xF000},
    //Noise generator (gain set, disabled, write 0x002E to enable)
    {0x00A0,  0x000E},
    //Tone generator (disabled)
    {0x0020,  0x0000},
};

/****************************************************/
// CLOCK DISABLE
/****************************************************/
#define AUDIO_CFG_CLKDIS_SZ                    3
static const AudMemReg AUDIO_CFG_CLKDIS[AUDIO_CFG_CLKDIS_SZ] = {
    //Disable all clocks
    {0x101,   CS_SYSCLK_DIS},    {0x120,   CS_DSPCLK_DIS},    {0x100,   CS_32KCLK_DIS},
};

/****************************************************/
// CLOCKS
/****************************************************/
#define AUDIO_CFG_CLK_SZ                    16
static const AudMemReg AUDIO_CFG_CLK[AUDIO_CFG_CLK_SZ] = {
    //CLOCKING: SRC=FLL1 DSPCLK,49.152MHz SRC=FLL1 SYSCLK
    //48/16/8 Sample rate,
    //147.456MHz DSP/DSP1, ,
    {0x102,   0x0003},    {0x103,   0x0012},    {0x104,   0x0011},
    {0x122,   0x24DD},    {0x124,   0x24DD},
    //FLL1: Configured for Fref of 1.536MHz
    //NK=64(nonFractional mode), no freerun, source AIF2BCLK,
    //gain 8, phase enabled  (10)
    {0x171,   0x0000},
    {0x173,   0x0000},    {0x174,   0x0001},    {0x172,   0x8040},    {0x175,   0x0000},
    {0x181,   0x0000},
    {0x176,   0x0009},    {0x179,   0x000C},    {0x17A,   0x2906},    {0x171,   0x0001},
    //GPIO: Pulldowns on MCLK1/2 (1)
    {0x14E,   0x0180},
};

/****************************************************/
// CLOCK ENABLE
/****************************************************/
#define AUDIO_CFG_CLKEN_SZ                    3
static const AudMemReg AUDIO_CFG_CLKEN[AUDIO_CFG_CLKEN_SZ] = {
    //CLOCK ENABLE
    {0x101,   CS_SYSCLK_EN},    {0x120,   CS_DSPCLK_EN},    {0x100,   CS_32KCLK_EN},
};

/****************************************************/
// CODEC ROUTING
/****************************************************/
#define AUDIO_CFG_ROUTE_SZ                  14
static const AudMemReg AUDIO_CFG_ROUTE[AUDIO_CFG_ROUTE_SZ] = {
    //LHPF1->DSP1L, LHPF2->DSP1R
    {0x940,   0x0060},      {0x948,   0x0061},   //DSP1L DSP1R
    //DSP1L->DRC1L, DSP1R->DRC1R
    {0x8C0,   0x0068},      {0x8C8,   0x0069}, //DSP1L DSP1R
    //AIF2 -> OUT5
    {0x6C2,   0x0028},      {0x6CA,   0x0029},
    //AIF3 -> OUT5
    {0x6C4,   0x0030},      {0x6CC,   0x0031},
#ifndef DSP_TEST_SETUP
    //IN3L->LHPF11, IN3R->LHPF21
    {0x900,   0x0014},      {0x908,   0x0015},
    //DRC1L->OUT5L, DRC1R->OUT5R //TODO reinstate
    //{0x6C0,   0x0058},      {0x6C8,   0x0059},
    {0x6C0,   0x0014},      {0x6C8,   0x0015},
#else
    //IN3LR->AIF1, AIF1->OUT5
    {0x700,   0x0014},      {0x708,   0x0015},
    {0x6C6,   0x0020},      {0x6CE,   0x0021},
#endif
    //Noise generator (to OUT) (write 0x000D), IN (to OUT) (write 0x0014/16), Tone (write 0x0004)
    {0x6C2,   0x000D},      {0x6CA,   0x000D},
};

/****************************************************/
// INPUT
/****************************************************/
#define AUDIO_CFG_MIC_SZ                    10
static const AudMemReg AUDIO_CFG_MIC[AUDIO_CFG_MIC_SZ] = {
    //IN3: HPF enable, OSR 3.072, 48kHz sample rate, clksource internal (5)
	{0x320,   0x8000},    {0x322,   0x0500},    {0x324,   0x8000},
    {0x323,   0x0000},    {0x327,   0x0000},
    //IN1-5: Enable inputs, Sample rate independent, HPF 40Hz (5)
    {0x300,   0x0030},    {0x30C,   0x0004},    {0x308,   0x0400},    {0x340,   0x0001},
    {0x311,   0x0380},
};

/****************************************************/
// PROCESSING
/****************************************************/
#define AUDIO_CFG_PROC_SZ                   40
static const AudMemReg AUDIO_CFG_PROC[AUDIO_CFG_PROC_SZ] = {
	//DRC/LHPF sampling rate
	{0xE00, 0x0000},
    //DRC1 Settings (5)
    {0xE81, 0x0929},      {0xE82, 0x6010},      {0xE83, 0x02F7},
    {0xE84, 0x0000},      {0xE80, 0x011B},
    //LHPF1 LHPF2
    {0xEC1, 0xFB25},      {0xEC0, 0x0001},
    {0xEC5, 0xFB25},      {0xEC4, 0x0001},
    //AIF1 Settings
    {0x500, 0x002D},      {0x501, 0x0000},
    {0x502, 0x0001},      {0x503, 0x0000},
    {0x504, 0x0002},      {0x506, 0x0040},
    {0x507, 0x1820},      {0x508, 0x1820},
#ifdef DSP_TEST_SETUP
    {0x519, 0x0003},      {0x51A, 0x0003},
#else
    {0x519, 0x0000},      {0x51A, 0x0000},
#endif
    //AIF2 Settings, I2S, SR1, 1.536M, Slave mode,
	//32cycles/frame 16bits/cycles per slot (@48kHz/16bit gives us 1.536MHz)
    {0x540, 0x000B},      {0x541, 0x0000},
    {0x542, 0x0000},      {0x543, 0x0000},
    {0x544, 0x0002},      {0x546, 0x0020},
    {0x547, 0x1010},      {0x548, 0x1010},
	{0x559, 0x0000},      {0x55A, 0x0000},
	//AIF3 Settings, I2S, SR1, 3.072M, Slave mode,
	//64cycles/frame 32cycles/24bits per slot
    {0x580, 0x000D},      {0x581, 0x0000},
    {0x582, 0x0000},      {0x583, 0x0000},
    {0x584, 0x0002},      {0x586, 0x0040},
    {0x587, 0x1820},      {0x588, 0x1820},
	{0x599, 0x0000},      {0x59A, 0x0000},
};

/****************************************************/
// OUTPUT
/****************************************************/
#define AUDIO_CFG_SPK_SZ                    6
static const AudMemReg AUDIO_CFG_SPK[AUDIO_CFG_SPK_SZ] = {
    //Enabled, ANC/HIFI disabled, Normal mode, PDM Mode A (4)
    {0x490,   0x0066},    {0x491,   0x0000},    {0x430,   0x0000},    {0x408,   0x0000},
    {0x434,   0x0000},
    {0x400,   0x0300},
};

/****************************************************/
// ADJUSTABLE SETTINGS
/****************************************************/


#endif
/****************************************************/
/****************************************************/
// FUNCTIONS
/****************************************************/
/****************************************************/
bool audioLoadDefaultsToNVM(AudioState *state)
{
#ifdef AUD_LOAD_CFG
    //Erase and initialise chunk
    if (memInitChunk(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND]) != MEM_OP_SUCCESS)
        return false;

    //Write elements
    if (!audioRAMToNVSLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND],
            AUDIO_CFG_RST, AUDIO_CFG_RST_SZ))
       return false;
    //audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], 0);

    if (!audioRAMToNVSLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND],
			AUDIO_CFG_INIT, AUDIO_CFG_INIT_SZ))
	   return false;
    //audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], 1);

    if (!audioRAMToNVSLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND],
			AUDIO_CFG_CLKDIS, AUDIO_CFG_CLKDIS_SZ))
	   return false;
    //audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], 2);

    if (!audioRAMToNVSLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND],
            AUDIO_CFG_CLK, AUDIO_CFG_CLK_SZ))
       return false;
    //audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], 3);

    if (!audioRAMToNVSLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND],
			AUDIO_CFG_CLKEN, AUDIO_CFG_CLKEN_SZ))
	   return false;
    //audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], 4);

    if (!audioRAMToNVSLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND],
            AUDIO_CFG_ROUTE, AUDIO_CFG_ROUTE_SZ))
       return false;
    //audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], 5);

    if (!audioRAMToNVSLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND],
            AUDIO_CFG_MIC, AUDIO_CFG_MIC_SZ))
       return false;
    //audioNVSToDSPLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND], 6);

    if (!audioRAMToNVSLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND],
            AUDIO_CFG_PROC, AUDIO_CFG_PROC_SZ))
       return false;

    if (!audioRAMToNVSLoadAudMemReg(state, &state->peri->mem.chunks[AUD_MEMCHUNK_CFG_IND],
            AUDIO_CFG_SPK, AUDIO_CFG_SPK_SZ))
       return false;

#endif

#ifdef AUD_LOAD_ADJ
    if (memInitChunk(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_ADJ_IND]) != MEM_OP_SUCCESS)
        return false;

    if (!audioRAMToNVSLoadAdjConfig(state))
        return false;

#endif

#ifdef AUD_LOAD_DSPCFG
    if (memInitChunk(&state->peri->mem, &state->peri->mem.chunks[AUD_MEMCHUNK_DSPCFG_IND]) != MEM_OP_SUCCESS)
        return false;

    if (!audioRAMToNVSLoadDSPConfig(state))
        return false;
#endif

    return true;
}
