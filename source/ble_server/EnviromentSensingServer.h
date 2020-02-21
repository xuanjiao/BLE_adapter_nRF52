#ifndef ENVIROMENT_SENSING_SERVER_H
#define ENVIROMENT_SENSING_SERVER_H

#include "ble/BLE.h"
#include <GattCharacteristic.h>
#include <GattService.h>
#include "events/EventQueue.h"
#include "SEGGER_RTT.h"
#include "sensor_type.h"

class EnviromentSensingServer{
    typedef EnviromentSensingServer Self;

    public:
        EnviromentSensingServer():
            _light_char(UUID_LIGHT_CHAR),
            _acc_char(UUID_ACCELERATE_CHAR){}

        void start(BLE &ble_interface,events::EventQueue& event_queue);

        // Update light value in ldr gatt characteristic.
        void update_sensor_value(sensor_type type,uint8_t light, char* time);

        void create_enviroment_sensing_service();
        
        class SensorCharacteristic : public GattCharacteristic{
        
            public:
             SensorCharacteristic(uint16_t uuid):
                GattCharacteristic(
                /* UUID */              uuid,
                /* Initial value */     0,
                /* Value size */        sizeof(uint8_t),
                /* Value capacity */    sizeof(uint8_t),
                /* Properties */        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE |
                                        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY |
                                        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE,
                /* Descriptors */       NULL,
                /* Num descriptors */   0,
                /* variable len */      false
                ){}
        };

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