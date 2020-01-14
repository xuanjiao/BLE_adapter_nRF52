
#include "ble/BLE.h"
#include <GattCharacteristic.h>
#include <GattService.h>
#include "SEGGER_RTT.h"

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

#define BLE_LDR_VALUE_SIZE  1
#define NUM_OF_CHAR         1

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
            printf("Adding LDR service.\r\n");
            _gattServer = &ble_interface.gattServer();
            GattCharacteristic *characteristics[] = {&_LDRgattCharacteristic};
            GattService LDRService(GattService::UUID_ENVIRONMENTAL_SERVICE,
                                    characteristics,
                                    NUM_OF_CHAR);

            ble_error_t error = _gattServer->addService(LDRService);
            if(error){
                printf("Error %u during adding LDR service.\r\n",error);
               // printf("sizeof char = %d",sizeof(characteristics));
                return;
            }
           // _event_queue->call_every(1000 /* ms */, &Demo,Demo::update_sensor_value,ble_interface);
           
        
            
        }
        void update_sensor_value(uint8_t light){
            printf("got light = %x\r\n",light);
        }

        // void update_sensor_value(LDR &ldr){
        //     printf("light = %x\r\n",ldr.getLight());
        // }

        void write_value_to_buffer(uint8_t value){
            _valueBytes[0] = value;
        }
        



};