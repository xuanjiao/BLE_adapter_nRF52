#include "EnviromentSensingServer.h"


#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

void EnviromentSensingServer::start(BLE &ble_interface,events::EventQueue& event_queue)
{
    _ble_interface = &ble_interface;
    _event_queue = &event_queue;
    _gattServer = &_ble_interface->gattServer();
    create_enviroment_sensing_service();
}

void EnviromentSensingServer::create_enviroment_sensing_service()
{
    GattCharacteristic *characteristics[] = {
        &_light_char,
        &_acc_char
    };

     // Register service.
    printf("Adding Enviroment sensing service.\r\n");
    GattService service(GattService::UUID_ENVIRONMENTAL_SERVICE,
                                    characteristics,
                                    NUM_OF_CHAR);

    ble_error_t error = _gattServer->addService(service);
    if(error){
        printf("Error %u during adding LDR service.\r\n",error);
          return;
    } 
}


        // Update light value in ldr gatt characteristic.
void EnviromentSensingServer::update_sensor_value(sensor_type type,uint8_t value,char* time){
    
    switch (type)
    {
    case sensor_type::light :
        printf("got light = %x, time %s\r\n",value,time);
        _gattServer->write( 
            _light_char.getValueHandle(),
            &value,
            sizeof(light)
        );
        break;
    case sensor_type::accumulate :
        printf("got accumulate = %x, time %s\r\n",value,time);
    default:
        break;
    }
    

}