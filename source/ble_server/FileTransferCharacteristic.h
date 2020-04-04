#ifndef FILE_TRANSFER_CHARACTERISTIC_H
#define FILE_TRANSFER_CHARACTERISTIC_H

extern int MAX_READ_BYTES;

#include <GattCharacteristic.h>

typedef struct File_st{
    int offset;
    char path[50];
}File_st;

class FileTransferCharacteristic : public GattCharacteristic{
        
    public:
        FileTransferCharacteristic(uint16_t uuid,uint8_t* initial):
            GattCharacteristic(
            /* UUID */              uuid,
            /* Initial value */     initial,
            /* Value size */        MAX_READ_BYTES,
            /* Value capacity */    MAX_READ_BYTES,
            /* Properties */        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
                                    | GattCharacteristic::Properties_t::BLE_GATT_CHAR_PROPERTIES_NOTIFY,
            /* Descriptors */       NULL,
            /* Num descriptors */   0,
            /* variable len */      false
            ){}
 
    private:

};

#endif