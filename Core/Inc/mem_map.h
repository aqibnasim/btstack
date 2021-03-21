/*! \file "mem_map.h"
   \brief Memory map. This file contains details about the different Memory Chunks
   stored in External NVS.
   Details are stored in the MemChunk array "MEMMAP_CHUNK".
*/

#ifndef MEM_MAP_H_
#define MEM_MAP_H_

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#include <memory/mem_peri.h>

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* CONSTANTS -----------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*!\var uint32_t MEM_TOTAL_SIZE
	Size of NVS in bytes
*/
static const uint32_t MEM_TOTAL_SIZE = 0x200000;

/*!\var uint32_t MEM_SECTOR_SIZE
	Size of NVS Sector in bytes
*/
static const uint32_t MEM_SECTOR_SIZE = 4096;

#define MEMCHUNK_TOTAL       10
static const MemChunk MEMMAP_CHUNK[MEMCHUNK_TOTAL] =
{
	//AUDIO: CODEC Configuration
	{
		1,
		0x1000
	},
	//AUDIO: CODEC Adjustable values
	{
		1,
		0x2000
	},
	//AUDIO: DSP Configurations
	{
		1,
		0x3000
	},
	//AUDIO: DSP Parameters
	{
		21,
		0x4000
	},
	//AUDIO: DSP Programs
	{
		231,
		0x19000
	},
	//POWERUI: Configuration
	{
		1,
		0x17E000
	},
	//POWERUI: LED
	{
		1,
		0x17F000
	},
	//SYSTEM: Factory backup
	{
		2,
		0x17B000
	},
	//SYSTEM: Details
	{
		1,
		0x17D000
	},
	//OAD Image
	{
		128,
		0x180000
	},
};

#define AUDCHUNK_TOTAL       1
static const MemChunk AUDMAP_CHUNK[AUDCHUNK_TOTAL] =
{
	//AUDIO CLIPS
	{
		512,
		0x0
	},
};

#endif /* MEM_MAP_H_ */
