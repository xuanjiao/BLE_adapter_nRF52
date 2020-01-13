
#include "ble/BLE.h"
#include <GattCharacteristic.h>
#include <GattService.h>
#include "SEGGER_RTT.h"

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

#define BLE_LDR_VALUE_SIZE  4

class LDRService{
    
    private:
    //const static uint16_t UUID_LDR_SERVICE = 0xA000;
    const static uint16_t UUID_LDR_CHARACTERISTIC = 0xA001;
    uint8_t _valueBytes[BLE_LDR_VALUE_SIZE];
    GattServer *_gattServer;
    GattCharacteristic _LDRgattCharacteristic;

    public:
        LDRService():
        _LDRgattCharacteristic(
                                    //UUID_LDR_SERVICE,
                                    /* UUID */              UUID_LDR_CHARACTERISTIC,
                                    /* Initial value */     _valueBytes,
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

        void start(BLE &ble_interface, events::EventQueue &event_queue){

            // Register service.
            printf("start adding LDR service.");
            _gattServer = &ble_interface.gattServer();
            GattCharacteristic *characteristics[] = {&_LDRgattCharacteristic};
            GattService LDRService(GattService::UUID_ENVIRONMENTAL_SERVICE,
                                    characteristics,
                                    sizeof(characteristics));

            ble_error_t error = _gattServer->addService(LDRService);
            if(error){
                printf("Error %u during adding LDR service.\r\n",error);
            }
        }




        void write_value_to_buffer(uint8_t value){
            _valueBytes[0] = value;
        }
        



};