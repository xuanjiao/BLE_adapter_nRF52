#include "LDRService.h"


#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

void LDRService::start(BLE &ble_interface){

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
void LDRService::update_sensor_value(uint8_t light,char* time){
    printf("got light = %x, time %s\r\n",light,time);
    _gattServer->write( 
    _LDRgattCharacteristic.getValueHandle(),
    &light,
    sizeof(light));
}