#include "EnviromentSensingServer.h"


#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

void EnviromentSensingServer::start(BLE &ble_interface,events::EventQueue& event_queue)
{
    _ble_interface = &ble_interface;
    _event_queue = &event_queue;
    _gattServer = &_ble_interface->gattServer();
    create_enviroment_sensing_service();
    create_file_transfer_service();
}

void EnviromentSensingServer::create_enviroment_sensing_service()
{

    GattCharacteristic* bs_char_list[] = {
            new SensorCharacteristic(uuids[Sensor_type::light]),
            new SensorCharacteristic(uuids[Sensor_type::magnetometer])
    };

     // Register service.
    int num = sizeof(bs_char_list)/sizeof(GattCharacteristic*);
    printf("Adding Enviroment sensing service with %d characteristics\r\n",num);
    
    GattService service(GattService::UUID_ENVIRONMENTAL_SERVICE,(GattCharacteristic**)bs_char_list,num);

    ble_error_t error = _gattServer->addService(service);
    if(error){
        printf("Error %u during adding Enviroment sensing service.\r\n",error);
          return;
    } 
}

void EnviromentSensingServer::create_file_transfer_service()
{
    
    for(int i = 0; i< num_of_file; i++){
        UUID uuid(file_char_uuid_base+i);
        uint8_t initial = i;
        chars[i] = new ReadWriteGattCharacteristic<uint8_t>(uuid,&initial);
        // printf("add uuid %x\n",uuid.getShortUUID());
    };

    GattService service(
            UUID::ShortUUIDBytes_t(file_service_uuid),
            (GattCharacteristic**)chars,
            num_of_file);

    ble_error_t error = _gattServer->addService(service);
    if(error){
        printf("Error %u during adding file transfer service.\r\n",error);
          return;
    } 

    printf("characteristic list:\n");
    for(int i = 0; i < num_of_file;i++){
        printf("[file no] %d [value attribute uuid] 0x%x [value handle] %d\n",
            i,
            chars[i]->getValueAttribute().getUUID().getShortUUID(),
            chars[i]->getValueHandle());
    }

   _gattServer->onDataRead(as_cb(&Self::when_data_read_by_client));
}

void EnviromentSensingServer::when_data_read_by_client(const GattReadCallbackParams* params)
{
    printf("data read by client. Connect handle %d data handle %d",params->connHandle,params->handle);
    for(int i = 0; i < num_of_file;i++){
        if(params->handle == chars[i]->getValueHandle())
            printf("file %d read by client\n",i);
    }
    
}


// Update light value in ldr gatt characteristic.
void EnviromentSensingServer::update_sensor_value(Sensor_type type,uint8_t value,char* time){
    
    // switch (type)
    // {
    // case light :
    //     printf("got light = %x, time %s\r\n",value,time);
    //     _gattServer->write( 
    //         _light_char.getValueHandle(),
    //         &value,
    //         sizeof(light)
    //     );
    //     break;
    // case magnetometer :
    //     printf("got magnetometer = %x, time %s\r\n",value,time);
    // default:
    //     break;
    // }
    
}


template<typename Arg>
FunctionPointerWithContext<Arg> EnviromentSensingServer::as_cb(void (Self::*member)(Arg))
{
    return makeFunctionPointer(this, member);
}