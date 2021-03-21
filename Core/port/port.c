#include "port.h"
#include "btstack.h"
#include "main.h"
#include "btstack_debug.h"
#include "btstack_chipset_cc256x.h"
#include "btstack_run_loop_embedded.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btstack_config.h"
#include <inttypes.h>




#include "btstack_audio.h"

#define __BTSTACK_FILE__ "port.c"

// include STM32 first to avoid warning about redefinition of UNUSED



#include "btstack_tlv.h"
#include "btstack_tlv_flash_bank.h"
#include "ble/le_device_db_tlv.h"
#include "classic/btstack_link_key_db_static.h"
#include "classic/btstack_link_key_db_tlv.h"
#include "hal_flash_bank_stm32.h"
//
#ifdef ENABLE_SEGGER_RTT
#include "SEGGER_RTT.h"
#endif

static btstack_tlv_flash_bank_t btstack_tlv_flash_bank_context;
static hal_flash_bank_stm32_t   hal_flash_bank_context;
#define HAL_FLASH_BANK_SIZE (128 * 512)
#define HAL_FLASH_BANK_0_ADDR 0x08040000
#define HAL_FLASH_BANK_1_ADDR 0x08060000


#define HAL_FLASH_BANK_0_SECTOR FLASH_SECTOR_6
#define HAL_FLASH_BANK_1_SECTOR FLASH_SECTOR_7

// UART configuration
static const hci_transport_config_uart_t config = {
    HCI_TRANSPORT_CONFIG_UART,
    115200,
    0,  // main baud rate = initial baud rate
    1,  // use flow control
    NULL
};




int btstack_main(int argc, const char ** argv);

void port_main(void){

	 btstack_memory_init();
	    btstack_run_loop_init(btstack_run_loop_embedded_get_instance());

	    hci_dump_open( NULL, HCI_DUMP_STDOUT );

		// init HCI
	    hci_init(hci_transport_h4_instance(btstack_uart_block_embedded_instance()), (void*) &config);
	    hci_set_chipset(btstack_chipset_cc256x_instance());

	   // MAX_NR_HCI_CONNECTIONS(0);
	    // setup TLV Flash Sector implementation
	    const hal_flash_bank_t * hal_flash_bank_impl = hal_flash_bank_stm32_init_instance(
	    		&hal_flash_bank_context,
	    		HAL_FLASH_BANK_SIZE,
				HAL_FLASH_BANK_0_SECTOR,
				HAL_FLASH_BANK_1_SECTOR,
				HAL_FLASH_BANK_0_ADDR,
				HAL_FLASH_BANK_1_ADDR);
	    const btstack_tlv_t * btstack_tlv_impl = btstack_tlv_flash_bank_init_instance(
	    		&btstack_tlv_flash_bank_context,
				hal_flash_bank_impl,
				&hal_flash_bank_context);
//
//	    // setup global tlv
	    btstack_tlv_set_instance(btstack_tlv_impl, &btstack_tlv_flash_bank_context);

	    // setup Link Key DB using TLV
	    const btstack_link_key_db_t * btstack_link_key_db = btstack_link_key_db_tlv_get_instance(btstack_tlv_impl, &btstack_tlv_flash_bank_context);
	    hci_set_link_key_db(btstack_link_key_db);

	    // setup LE Device DB using TLV
	    le_device_db_tlv_configure(btstack_tlv_impl, &btstack_tlv_flash_bank_context);


#ifdef HAVE_HAL_AUDIO
    // setup audio
   	btstack_audio_sink_set_instance(btstack_audio_embedded_sink_get_instance());
    btstack_audio_source_set_instance(btstack_audio_embedded_source_get_instance());
#endif

    hci_set_chipset(btstack_chipset_cc256x_instance());
		// hand over to btstack embedded code
	    btstack_main(0, NULL);

	    // go
	    btstack_run_loop_execute();
	    //hci_dump_open( NULL, HCI_DUMP_STDOUT );

}
