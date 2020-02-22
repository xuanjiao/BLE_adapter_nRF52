#include "GattClientProcess.h"

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)


void GattClientProcess::init(BLE &ble_interface, events::EventQueue &event_queue)
{
    _ble_interface = &ble_interface;

    _event_queue = &event_queue;
    
    _gattClient = &_ble_interface->gattClient();

 

  //  _ble_interface->gap().setEventHandler(this);
    
  //  _ble_interface->gap().onConnection(this,&GattClientProcess::when_connection);

    
    printf("Init BLE Gatt Client finished.\n");
    
}

void GattClientProcess::when_char_data_read(const GattReadCallbackParams* params)
{
    printf("read %d bytes char data from handle %d\n",params->len,params->handle);

    // Check read status
    if(params->status != BLE_ERROR_NONE){
        printf("Status %x during read data. Error code %x.\n",params->status,params->error_code);
        return;
    }

    // read data from callback parameter
    for(int i = 0; i < params->len;i++){
        printf("0x%02x ",params->data[i]);
    }
    printf("\n");

    typedef struct date_time{
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint8_t day_of_week;
    }DateTime;
    
    
    uint8_t data[params->len];
    memcpy(data,params->data,params->len);

    DateTime time;
    time.year           =  *((uint16_t*)data);
    time.month          = data[2];
    time.day            = data[3];
    time.hour           = data[4];
    time.minute         = data[5];
    time.second         = data[6];
    time.day_of_week    = data[7];

    printf("%d-%d-%d %d:%d:%d %d\n",
            time.year,
            time.month,
            time.day,
            time.hour,
            time.minute,
            time.second,
            time.day_of_week);
}

// Called when connection attempt ends or an advertising device has been connected.
void GattClientProcess::onConnectionComplete(const ble::ConnectionCompleteEvent &event)
{
    printf("--On connection complete.\n");
    _connection_handle = event.getConnectionHandle();
    _event_queue->call(mbed::callback(this,&Self::start));
}

void GattClientProcess::start()
{
    _gattClient->onServiceDiscoveryTermination(as_cb(&Self::when_service_discovery_ends));

    _gattClient->onDataRead(as_cb(&Self::when_char_data_read));

    ble_error_t error = _gattClient->launchServiceDiscovery(
        _connection_handle,
        as_cb(&Self::when_service_discovered),
        as_cb(&Self::when_characteristic_discovered),
       UUID::ShortUUIDBytes_t(GattService::UUID_CURRENT_TIME_SERVICE),
       UUID::ShortUUIDBytes_t(GattCharacteristic::UUID_CURRENT_TIME_CHAR)
    );

    if(error){
        printf("Error %u during launch service discovery\r\n");
        return;
    }
        
    printf("GATT Client start.Discovering services....\r\n");

}

void GattClientProcess::when_service_discovery_ends(ble::connection_handle_t connection_handle)
{
    printf("All services and characteristics discovered, process them.\r\n");
    if(_CTC_handle == GattAttribute::INVALID_HANDLE){
        printf("No ctc handle found.\n");
        return;
    }

    // When all serviced discovered, use handle to read characteristic value.
    ble_error_t error = _gattClient->read(
        connection_handle,  // connection handle
        _CTC_handle,        // attribute handle
        0                   //offset
    );

    if(error){
        printf("Error %u during read ble attributes.\n");
    }

}


// The GattClient invokes this function when a service has been discovered.
void GattClientProcess::when_service_discovered(const DiscoveredService* discovered_service)
{
    uint16_t uuid = *((const uint16_t*)discovered_service->getUUID().getBaseUUID());
    int start_handle = discovered_service->getStartHandle();
    int end_handle = discovered_service->getEndHandle();
    printf("Service discovered: uuid: %x, start handle %d, end handle\n",uuid,start_handle,end_handle);
}

void GattClientProcess::when_characteristic_discovered(const DiscoveredCharacteristic* discovered_characteristic)
{
    uint16_t uuid = *((const uint16_t*)discovered_characteristic->getUUID().getBaseUUID());
    int handle = discovered_characteristic->getValueHandle();
    switch (uuid)
    {
    case GattCharacteristic::UUID_CURRENT_TIME_CHAR:
                printf("**Current time Characteristic found. Value handle %d\n",handle);
                _CTC_handle = handle;
        break;
    
    default:
        printf("Other Characteristic discovered: uuid: %x, value handle %d\n",uuid,handle);
        break;
    }
    
}


// Stop the discovery process and clean the instance.
void GattClientProcess::stop()
{
    if (!_gattClient) {
         return;
         }

    // unregister event handlers
    _gattClient->onServiceDiscoveryTermination(NULL);

    // Clear up instance
    _gattClient = NULL;

    _connection_handle = 0;

    printf("Client process stopped.\r\n");
}

template<typename Arg>
FunctionPointerWithContext<Arg> GattClientProcess::as_cb(void (Self::*member)(Arg))
{
    return makeFunctionPointer(this, member);
}