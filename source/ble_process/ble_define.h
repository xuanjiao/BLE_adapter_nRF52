#ifndef _BLE_DEFINE_H
#define _BLE_DEFINE_H

#include "sensor_type.h"
#include "UUID.h"
#include "GattAttribute.h"
#include "BLE.h"
#include "gap/Gap.h"
#include "Gap.h"
#include "GattClient.h"
#include "GattCharacteristic.h"
#include "GapAdvertisingParams.h"
#include "GapAdvertisingData.h"
#include "DiscoveredService.h"
#include "DiscoveredCharacteristic.h"
#include "CharacteristicDescriptorDiscovery.h"
#include "global.h"

static const int MAX_SENSOR_DATA_LEN = 3;

static const int MAX_SENSOR_CMD_LEN = 2;

static const int MAX_TYPE_NAME_LEN = 5;

static const UUID UUID_CURRENT_TIME_CHAR = UUID(GattCharacteristic::UUID_CURRENT_TIME_CHAR);

/* Duration of each mode in milliseconds */
static const size_t MODE_DURATION_MS      = 10000;

/* Time between each mode in milliseconds */
static const size_t TIME_BETWEEN_MODES_MS = 500;

/* how long to wait before disconnecting in milliseconds */
static const size_t CONNECTION_DURATION   = 3000;

static const int LED_BLINK_INTERVAL_MS = 500;

static const uint16_t MAX_ADVERTISING_PAYLOAD_SIZE = 251;

static const uint16_t CC2650_ADV_UUID = 0xAA80;

static const int ADDR_LEN = 6;


typedef struct {
    Sensor_type type;
    UUID uuid_data;
    UUID uuid_config;
}Sensor_char_uuid_t;

typedef struct {
    Sensor_type type;
    uint8_t cmd[MAX_SENSOR_CMD_LEN];
    uint8_t len;
}Sensor_cmd_t;

typedef struct {
    Sensor_type type;
    GattAttribute::Handle_t config_handle;
    GattAttribute::Handle_t data_handle;
    int16_t data[MAX_SENSOR_DATA_LEN];
    uint8_t len;
}Sensor_char_t;

typedef struct {
    Sensor_type type;
    GattAttribute::Handle_t config_handle;
    GattAttribute::Handle_t data_handle;
}Sensor_handle_t;

/**
 *  device infoamation include
 *  connection handle
 *  wheather it has current time info/ measurement results
 *  MAC address xx:xx:xx:xx:xx:xx
 *  handle sets used to read measurement results.
 */
typedef struct {
    ble::connection_handle_t connection_handle;
    bool is_CTC;
    bool is_beacon;
    uint8_t address[ADDR_LEN+1];
    int num_of_chars;
    union {
        Sensor_char_t sensor_chars[3];
        GattAttribute::Handle_t time_value_handle;
    }chars;
}Device_t;

typedef struct date_time{
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint8_t day_of_week;
}DateTime_t;

typedef struct {
    ble::advertising_type_t type;
    ble::adv_interval_t min_interval;
    ble::adv_interval_t max_interval;       
} AdvParams_t;

typedef struct {
    ble::scan_interval_t interval;
    ble::scan_window_t window;
    ble::scan_duration_t duration;
    bool active;
} ScanParams_t;

typedef struct {
    UUID uuid;
    uint8_t value;
} AdvServiceData_t;

static const AdvParams_t adv_params = {
    ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
    ble::adv_interval_t(40),    // 0.625us x25 = 40us
    ble::adv_interval_t(80)     //  80 us
};

static const ScanParams_t scan_params = {
    ble::scan_interval_t(160),  // 0.625us x  160 = 100us
    ble::scan_window_t(100),    //  625us x 100 = 6.25ms
    ble::scan_duration_t(0),     // scan never ends 
    true    // active scanning flag.
};


#endif