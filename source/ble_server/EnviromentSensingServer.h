#ifndef ENVIROMENT_SENSING_SERVER_H
#define ENVIROMENT_SENSING_SERVER_H

#include "ble/BLE.h"
#include <GattCharacteristic.h>
#include <GattService.h>
#include "events/EventQueue.h"
#include "SEGGER_RTT.h"
#include "sensor_type.h"
#include "UUID.h"
#include "SensorCharacteristic.h"

static const uint16_t uuids[] = {
    0xA000,     // unknown
    0xA001,     // light
    0xA002     // magnetometer 
};

static uint16_t file_char_uuid_base = 0xB100;

static const int num_of_file = 2;

static const uint16_t file_service_uuid = 0xB000;

class EnviromentSensingServer{
    typedef EnviromentSensingServer Self;

    public:
        EnviromentSensingServer(){}

        void start(BLE &ble_interface,events::EventQueue& event_queue);

        // Update light value in ldr gatt characteristic.
        void update_sensor_value(Sensor_type type,uint8_t light, char* time);

        void create_enviroment_sensing_service();

        void create_file_transfer_service();
        
        void when_data_read_by_client(const GattReadCallbackParams* params);
        // ~EnviromentSensingServer(){
        //     for(size_t i = 0; i < sizeof(bs_char_list);i++){
        //         delete bs_char_list[i];
        //     }
        // }

        private:

        // GattCharacteristic** head_char;

        GattServer *_gattServer;

        GattCharacteristic* chars[num_of_file];

        BLE *_ble_interface;

        events::EventQueue *_event_queue;

         template<typename Arg>
        FunctionPointerWithContext<Arg> as_cb(void (Self::*member)(Arg));
};

#endif