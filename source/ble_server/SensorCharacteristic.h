#ifndef SENSOR_CHARACTERISTIC_H
#define SENSOR_CHARACTERISTIC_H

#include <GattCharacteristic.h>

class SensorCharacteristic : public GattCharacteristic{
        
    public:
        SensorCharacteristic(uint16_t uuid):
            GattCharacteristic(
            /* UUID */              uuid,
            /* Initial value */     0,
            /* Value size */        sizeof(uint8_t),
            /* Value capacity */    8,
            /* Properties */        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
            /* Descriptors */       NULL,
            /* Num descriptors */   0,
            /* variable len */      false
            ){}
 
    private:

};

#endif