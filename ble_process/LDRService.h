
#include "ble/BLE.h"
#include <GattCharacteristic.h>
#include <GattService.h>
#include "events/EventQueue.h"
#include "SEGGER_RTT.h"
#include "sensor_type.h"


class LDRService{
    
    private:
        const static uint16_t UUID_LDR_CHARACTERISTIC = 0xA001;

        const static int BLE_LDR_VALUE_SIZE = 1;

        const static int NUM_OF_CHAR = 1;

        GattServer *_gattServer;

        GattCharacteristic _LDRgattCharacteristic;

        BLE *_ble_interface;

        events::EventQueue *_event_queue;

    public:
        LDRService():
            _LDRgattCharacteristic(
                                    //UUID_LDR_SERVICE,
                                    /* UUID */              UUID_LDR_CHARACTERISTIC,
                                    /* Initial value */     0,
                                    /* Value size */        BLE_LDR_VALUE_SIZE,
                                     /* Value capacity */   BLE_LDR_VALUE_SIZE,
                                    /* Properties */        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
                                                            | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY,
                                    /* Descriptors */       NULL,
                                    /* Num descriptors */   0,
                                    /* variable len */      false
        ),_gattServer(NULL)
        {

        }

        void start(BLE &ble_interface,events::EventQueue& event_queue);

        // Update light value in ldr gatt characteristic.
        void update_sensor_value(sensor_type type,uint8_t light, char* time);

};