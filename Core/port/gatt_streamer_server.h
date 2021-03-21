
// C:\Users\Ahsan\Desktop\gatt_streamer_server.h generated from btstack\example\gatt_streamer_server.gatt for BTstack
// it needs to be regenerated when the .gatt file is updated. 

// To generate C:\Users\Ahsan\Desktop\gatt_streamer_server.h:
// C:\Users\Ahsan\Desktop\Job\Projects\STM-32-F411\btstack\tool\compile_gatt.py btstack\example\gatt_streamer_server.gatt C:\Users\Ahsan\Desktop\gatt_streamer_server.h

// att db format version 1

// binary attribute representation:
// - size in bytes (16), flags(16), handle (16), uuid (16/128), value(...)

#include <stdint.h>

// Reference: https://en.cppreference.com/w/cpp/feature_test
#if __cplusplus >= 200704L
constexpr
#endif
const uint8_t profile_data[] =
{
    // ATT DB Version
    1,



	// 0x0007 PRIMARY_SERVICE-A31799e0-847b-4af7-b2bc-c15507b531b
	0x18, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x28,
	0x1B, 0x3A, 0xB5, 0x07, 0x55, 0xC1, 0xBC, 0xB2, 0xF7, 0x4A, 0x7B, 0x84, 0xe0, 0x99, 0x17, 0xA3,

    // Test Characteristic A, write_without_response as well as notify
    // 0x0008 CHARACTERISTIC-A31799e6-847b-4af7-b2bc-c15507b531b-WRITE_WITHOUT_RESPONSE | NOTIFY | DYNAMIC
    0x1b, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28,

	0x14, 0x09, 0x00,
	0x1B, 0x3A, 0xB5, 0x07, 0x55, 0xC1, 0xBC, 0xB2, 0xF7, 0x4A, 0x7B, 0x84, 0xe6, 0x99, 0x17, 0xA3,

	// 0x0009 VALUE-0000FF11-0000-1000-8000-00805F9B34FB-WRITE_WITHOUT_RESPONSE | NOTIFY | DYNAMIC-''
    // WRITE_ANYBODY
    0x16, 0x00, 0x04, 0x03, 0x09, 0x00, 0xfb, 0x34,
	0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x11, 0xff, 0x00, 0x00,

////////////////////////////////////////////////


	// 0x000d PRIMARY_SERVICE-A8470000-B7FE-4BD4-9C81-DEA848EBDEB3
	0x18, 0x00, 0x02, 0x00, 0x0d, 0x00, 0x00, 0x28,
	0xB3, 0xde, 0xeb, 0x48, 0xa8, 0xde, 0x81, 0x9C, 0xd4, 0x4b, 0xfe, 0xb7, 0x00, 0x00, 0x47, 0xa8,

	// Test Characteristic A, write_without_response as well as notify
	// 0x000e CHARACTERISTIC-0000FF11-0000-1000-8000-00805F9B34FB-WRITE_WITHOUT_RESPONSE | NOTIFY | DYNAMIC
	0x1b, 0x00, 0x02, 0x00, 0x0e, 0x00, 0x03, 0x28,

	0x14, 0x0f, 0x00,
	0xB3, 0xde, 0xeb, 0x48, 0xa8, 0xde, 0x81, 0x9C, 0xd4, 0x4b, 0xfe, 0xb7, 0x03, 0x00, 0x47, 0xa8,


	// 0x0010 VALUE-0000FF11-0000-1000-8000-00805F9B34FB-WRITE_WITHOUT_RESPONSE | NOTIFY | DYNAMIC-''
	    // WRITE_ANYBODY
	 0x16, 0x00, 0x04, 0x03, 0x0f, 0x00, 0xfb, 0x34,
	0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x11, 0xff, 0x00, 0x00,


	// END
    0x00, 0x00,
}; // total size 126 bytes 


//
// list service handle ranges
//
#define ATT_SERVICE_GAP_SERVICE_START_HANDLE 0x0001
#define ATT_SERVICE_GAP_SERVICE_END_HANDLE 0x0003
#define ATT_SERVICE_GAP_SERVICE_01_START_HANDLE 0x0001
#define ATT_SERVICE_GAP_SERVICE_01_END_HANDLE 0x0003
#define ATT_SERVICE_GATT_SERVICE_START_HANDLE 0x0004
#define ATT_SERVICE_GATT_SERVICE_END_HANDLE 0x0006
#define ATT_SERVICE_GATT_SERVICE_01_START_HANDLE 0x0004
#define ATT_SERVICE_GATT_SERVICE_01_END_HANDLE 0x0006
#define ATT_SERVICE_0000FF10_0000_1000_8000_00805F9B34FB_START_HANDLE 0x0007
#define ATT_SERVICE_0000FF10_0000_1000_8000_00805F9B34FB_END_HANDLE 0x000d
#define ATT_SERVICE_0000FF10_0000_1000_8000_00805F9B34FB_01_START_HANDLE 0x0007
#define ATT_SERVICE_0000FF10_0000_1000_8000_00805F9B34FB_01_END_HANDLE 0x000d

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_GAP_DEVICE_NAME_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_GATT_DATABASE_HASH_01_VALUE_HANDLE 0x0006
#define ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE 0x0009
#define ATT_CHARACTERISTIC_A31799E0_847B_4AF7_B2bC_C15507b531B_MEMORY_CONFIGURATION_HANDLE 0x0009
#define ATT_CHARACTERISTIC_0000FF12_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE 0x000c
#define ATT_CHARACTERISTIC_A8470003_B7FE_4BD4_9C81_DEA848EBDEB3_DEVICE_CONFIGURATION_HANDLE 0x000f
