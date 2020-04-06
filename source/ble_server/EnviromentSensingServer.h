#ifndef ENVIROMENT_SENSING_SERVER_H
#define ENVIROMENT_SENSING_SERVER_H

#include "mbed.h"
#include "ble/BLE.h"
#include <GattCharacteristic.h>
#include <GattService.h>
#include "events/EventQueue.h"
#include "SEGGER_RTT.h"
#include "sensor_type.h"
#include "UUID.h"
#include "global.h"
#include "SensorCharacteristic.h"
#include "platform/SharedPtr.h"
#include "FileTransferCharacteristic.h"

static const uint16_t uuids[] = {
    0xA000,     // unknown
    0xA001,     // light
    0xA002     // magnetometer 
};

static uint16_t file_char_uuid_base = 0xB100;

static const int MAX_CHARACTERISTIC_NUM = 10;

static const uint16_t file_service_uuid = 0xB000;


static File_st file_buff[MAX_CHARACTERISTIC_NUM] = {};

class EnviromentSensingServer{
    typedef EnviromentSensingServer Self;

    public:
        EnviromentSensingServer():_num_of_file(0){}

        void start(BLE &ble_interface,events::EventQueue& event_queue);

        // Update light value in ldr gatt characteristic.
        void update_sensor_value(Sensor_type type,uint8_t light, char* time);

        void create_enviroment_sensing_service();

        void create_file_transfer_service();

        void create_file_transfer_characteristics();
        
        void when_receive_read_request(GattReadAuthCallbackParams* params);

        void when_data_read_by_client(const GattReadCallbackParams* params);
    
        private:

        int _num_of_file;

        GattServer *_gattServer;

        GattCharacteristic* file_chars_buff[MAX_CHARACTERISTIC_NUM];

        BLE *_ble_interface;

        events::EventQueue *_event_queue;

         template<typename Arg>
        FunctionPointerWithContext<Arg> as_cb(void (Self::*member)(Arg));
};

#endif