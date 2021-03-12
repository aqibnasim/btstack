//
// btstack_config.h for metric calculation
//

#ifndef BTSTACK_CONFIG_H
#define BTSTACK_CONFIG_H

// Port related features
#define HAVE_BTSTACK_STDIN

// BTstack features that can be enabled
#define ENABLE_ATT_DELAYED_RESPONSE
#define ENABLE_BLE
#define ENABLE_CLASSIC
#define ENABLE_HFP_WIDE_BAND_SPEECH
#define ENABLE_L2CAP_ENHANCED_RETRANSMISSION_MODE
#define ENABLE_LE_CENTRAL
#define ENABLE_LE_DATA_CHANNELS
#define ENABLE_LE_DATA_LENGTH_EXTENSION
#define ENABLE_LE_PERIPHERAL
#define ENABLE_LE_SECURE_CONNECTIONS
#define ENABLE_LOG_ERROR
#define ENABLE_LOG_INFO
#define ENABLE_MICRO_ECC_FOR_LE_SECURE_CONNECTIONS
#define ENABLE_SCO_OVER_HCI
#define ENABLE_SDP_DES_DUMP

// BTstack configuration. buffers, sizes, ...
#define HCI_ACL_PAYLOAD_SIZE (1691 + 4)
#define HCI_INCOMING_PRE_BUFFER_SIZE 14 // sizeof BNEP header, avoid memcpy

#define MAX_NR_LE_DEVICE_DB_ENTRIES    4

#define MAX_ATT_DB_SIZE 1024

#define NVM_NUM_DEVICE_DB_ENTRIES     16
#define NVM_NUM_LINK_KEYS     16

// Mesh Configuration
#define ENABLE_MESH
#define ENABLE_MESH_ADV_BEARER
#define ENABLE_MESH_GATT_BEARER
#define ENABLE_MESH_PB_ADV
#define ENABLE_MESH_PB_GATT
#define ENABLE_MESH_PROVISIONER
#define ENABLE_MESH_PROXY_SERVER

#define MAX_NR_MESH_SUBNETS            2
#define MAX_NR_MESH_TRANSPORT_KEYS    16
#define MAX_NR_MESH_VIRTUAL_ADDRESSES 16

// allow for one NetKey update
#define MAX_NR_MESH_NETWORK_KEYS      (MAX_NR_MESH_SUBNETS+1)

//
//
// btstack_config.h for STM32F103RB Nucleo + TI CC256B port
//

// Port related features
#define HAVE_BTSTACK_STDIN
#define HAVE_EMBEDDED_TIME_MS
//#define HAVE_HAL_AUDIO

// BTstack features that can be enabled
#define ENABLE_BLE
#define ENABLE_CC256X_BAUDRATE_CHANGE_FLOWCONTROL_BUG_WORKAROUND
#define ENABLE_CLASSIC
#define ENABLE_HFP_WIDE_BAND_SPEECH
#define ENABLE_LE_CENTRAL
#define ENABLE_LE_DATA_CHANNELS
#define ENABLE_LE_PERIPHERAL
#define ENABLE_LOG_ERROR
#define ENABLE_PRINTF_HEXDUMP
//#define ENABLE_SCO_OVER_HCI
#define ENABLE_SCO_STEREO_PLAYBACK
//#define ENABLE_SEGGER_RTT

// BTstack configuration. buffers, sizes, ...
#define HCI_ACL_PAYLOAD_SIZE (1691 + 4)
#define MAX_NR_AVDTP_CONNECTIONS 1
#define MAX_NR_AVDTP_STREAM_ENDPOINTS 1
#define MAX_NR_AVRCP_CONNECTIONS 1
#define MAX_NR_BNEP_CHANNELS 1
#define MAX_NR_BNEP_SERVICES 1
#define MAX_NR_BTSTACK_LINK_KEY_DB_MEMORY_ENTRIES  2
#define MAX_NR_GATT_CLIENTS 1
#define MAX_NR_HCI_CONNECTIONS 2
#define MAX_NR_HFP_CONNECTIONS 1
#define MAX_NR_L2CAP_CHANNELS  4
#define MAX_NR_L2CAP_SERVICES  3
#define MAX_NR_RFCOMM_CHANNELS 1
#define MAX_NR_RFCOMM_MULTIPLEXERS 1
#define MAX_NR_RFCOMM_SERVICES 1
#define MAX_NR_SERVICE_RECORD_ITEMS 4
#define MAX_NR_SM_LOOKUP_ENTRIES 3
#define MAX_NR_WHITELIST_ENTRIES 1

// Link Key DB and LE Device DB using TLV on top of Flash Sector interface
#define NVM_NUM_DEVICE_DB_ENTRIES 16
#define NVM_NUM_LINK_KEYS 16

#endif

