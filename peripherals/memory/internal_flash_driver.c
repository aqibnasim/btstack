#include "memory/internal_flash_driver.h"
#include "stm32f4xx_hal.h"

#define FLASH_WRITE_SIZE	4

bool FlashWrite(volatile uint32_t *flash, const uint8_t *data, uint32_t count)
{
  //Calculate data end (in terms of 64bit writes)
  uint8_t *address = flash;
  size_t addrint = address;
  size_t data_s = 0;
  if ((addrint % 4) != 0)
  {
	  data_s = addrint % 4;
  }
  uint8_t const *data_e;
  uint8_t data_r;
  data_r = (count - data_s) % FLASH_WRITE_SIZE;
  data_e = (data + data_s) + (count - data_s - data_r);

  FLASH_WaitForLastOperation(50000);

  uint8_t *data_ws = data;
  while (data_s)
  {
	  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,
			  address, *data_ws) == HAL_OK)
	  {
		  data_ws++;
		  address++;
		  data_s--;
	  }
	  else
		  return false;
  }

  uint32_t *data_w = (uint32_t *)data_ws;

  while (data_w < data_e)
  {
	  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
			  address, *data_w) == HAL_OK)
	  {
		  data_w++;//  += FLASH_WRITE_SIZE;
		  address += FLASH_WRITE_SIZE;
	  }
	  else
		  return false;
  }
  
  uint8_t *data_wr = (uint8_t *)data_w;

  while (data_r)
  {
	  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,
			  address, *data_wr) == HAL_OK)
	  {
		  data_wr++;
		  address++;
		  data_r--;
	  }
	  else
		  return false;
  }
  
  return true;
}

bool FlashUnlock()
{
  if (HAL_FLASH_Unlock() == HAL_OK)
	  return true;
  else
	  return false;
}

bool FlashLock()
{
  if (HAL_FLASH_Lock() == HAL_OK)
	  return true;
  else
	  return false;
}
