
#include "ble/BLE.h"
#include <GattCharacteristic.h>
#include <GattService.h>
#include "SEGGER_RTT.h"

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

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

        void start(BLE &ble_interface){

            // Register service.
            printf("Adding LDR service.\r\n");
            _gattServer = &ble_interface.gattServer();
            GattCharacteristic *characteristics[] = {&_LDRgattCharacteristic};
            GattService LDRService(GattService::UUID_ENVIRONMENTAL_SERVICE,
                                    characteristics,
                                    NUM_OF_CHAR);

            ble_error_t error = _gattServer->addService(LDRService);
            if(error){
                printf("Error %u during adding LDR service.\r\n",error);
                return;
            }      
        }

        // Update light value in ldr gatt characteristic.
        void update_sensor_value(uint8_t light){
            printf("got light = %x\r\n",light);
            _gattServer->write( 
                _LDRgattCharacteristic.getValueHandle(),
                &light,
                sizeof(light));

        }

};