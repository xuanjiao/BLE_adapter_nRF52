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

static SensorCharacteristic* char_list[] = {
    new SensorCharacteristic(uuids[Sensor_type::light]),
    new SensorCharacteristic(uuids[Sensor_type::magnetometer])
};

class EnviromentSensingServer{
    typedef EnviromentSensingServer Self;

    public:
        EnviromentSensingServer():
            _light_char(UUID_LIGHT_CHAR),
            _acc_char(UUID_ACCELERATE_CHAR){}

        void start(BLE &ble_interface,events::EventQueue& event_queue);

        // Update light value in ldr gatt characteristic.
        void update_sensor_value(Sensor_type type,uint8_t light, char* time);

        void create_enviroment_sensing_service();

        ~EnviromentSensingServer(){
            for(int i = 0; i < sizeof(char_list);i++){
                delete char_list[i];
            }
        }

        private:
        
        const static uint16_t UUID_LIGHT_CHAR = 0xA001;
        const static uint16_t UUID_ACCELERATE_CHAR = 0xA00;


        const static int BLE_LDR_VALUE_SIZE = 1;

        const static int NUM_OF_CHAR = 1;

        GattServer *_gattServer;

        BLE *_ble_interface;

        events::EventQueue *_event_queue;

        SensorCharacteristic _light_char;
        SensorCharacteristic _acc_char;

};

#endif