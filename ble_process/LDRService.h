
#include "ble/BLE.h"
#include <GattCharacteristic.h>
#include <GattService.h>
#include "SEGGER_RTT.h"

#define BLE_LDR_VALUE_SIZE  1
#define NUM_OF_CHAR         1

class LDRService{
    
    private:
        const static uint16_t UUID_LDR_CHARACTERISTIC = 0xA001;
        GattServer *_gattServer;
        GattCharacteristic _LDRgattCharacteristic;

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

        void start(BLE &ble_interface);

        // Update light value in ldr gatt characteristic.
        void update_sensor_value(uint8_t light);

};