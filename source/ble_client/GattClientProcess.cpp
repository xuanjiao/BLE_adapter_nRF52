#include "GattClientProcess.h"
#include "mbed_rtc_time.h"
#include <ctime>

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)


void GattClientProcess::init(BLE &ble_interface, events::EventQueue &event_queue)
{
    _ble_interface = &ble_interface;

    _event_queue = &event_queue;
    
    _gattClient = &_ble_interface->gattClient();
 
    printf("Init BLE Gatt Client finished.\n");
        
    _gattClient->onServiceDiscoveryTermination(as_cb(&Self::when_service_discovery_ends));

    _gattClient->onDataRead(as_cb(&Self::when_char_data_read));
}

void GattClientProcess::start(device_t new_device)
{
    devices.insert(make_pair(new_device.connection_handle,new_device));
    printf("Get connection handle %d Discovering service...",new_device.connection_handle);

    // launch service discovery  peer
    ble_error_t error = _gattClient->launchServiceDiscovery(
        new_device.connection_handle,
        as_cb(&Self::when_service_discovered),
        as_cb(&Self::when_characteristic_discovered)
    );

    if(error){
        printf("Error %d\r\n",error);
        _event_queue->call(this,&Self::stop);
        return;
    }
        
    printf("OK\n");

}

void GattClientProcess::when_char_data_read(const GattReadCallbackParams* params)
{
        // Check read status
    if(params->status != BLE_ERROR_NONE){
        printf("Status %x during read data from handle %d. Error code %x.\n",
                params->status,
                params->handle,
                params->error_code);
        _event_queue->call(this,&Self::stop);        
        return;
    }

    // get attribute handle, data and data length 
    GattAttribute::Handle_t handle = params->handle;
    const uint16_t len = params->len;
    const uint8_t* p_data = params->data;
    
    printf("read %d bytes from handle %d: \n",len,handle);

    // read data from callback parameter
    for(uint16_t i = 0; i < len;i++){
        printf("0x%02x ",p_data[i]);
    }
    printf("\n");

    device_t &dev = devices[handle];

    // if device has CTS
    if(dev.is_CTS){
        setRTC(p_data,len);
    }

}

void GattClientProcess::when_service_discovery_ends(const ble::connection_handle_t connection_handle)
{
    ble_error_t error;
    printf("Connection hande %d. All services and characteristics discovered, process them.\r\n",connection_handle);
    
    device_t &dev = devices[connection_handle];
    
    print_device_info(dev);

    if(dev.is_CTS){
        // if peer is a CTS device, read its time value
        printf("read time from handle... %d\n",dev.time_handle);
        error = _gattClient->read(
            connection_handle,  // connection handle
            dev.time_handle,    // attribute handle
            0                   //offset
        );
    }else if(dev.is_beacon){
            // if peer is a CTS device, rhead its ligt value
            printf("read light from handle... %d\n",dev.light_handle);
            error = _gattClient->read(
            connection_handle,  // connection handle
            dev.light_handle,    // attribute handle
            0                   //offset
        );
    }

    // When all serviced discovered, use handle to read characteristic value.


    if(error){
        printf("Error %u during read ble attributes.\n");
        _event_queue->call(this,&Self::stop); 
    }

}


// The GattClient invokes this function when a service has been discovered.
void GattClientProcess::when_service_discovered(const DiscoveredService* discovered_service)
{
    printf("Service discovered: uuid: ");
    print_uuid(discovered_service->getUUID());
    
    int start_handle = discovered_service->getStartHandle();
    int end_handle = discovered_service->getEndHandle();
    printf("start handle %d, end handle %d\n",start_handle,end_handle);
}

void GattClientProcess::when_characteristic_discovered(const DiscoveredCharacteristic* discovered_characteristic)
{


    // uint16_t uuid = *((const uint16_t*)discovered_characteristic->getUUID().getBaseUUID());
    ble::connection_handle_t connHandle = discovered_characteristic->getConnectionHandle();
    UUID uuid = discovered_characteristic->getUUID();
    int handle = discovered_characteristic->getValueHandle();
    
    // find device object with related connectionhandle
    device_t &dev = devices[connHandle];

    printf("characteristic found,[char uuid]");
    print_uuid(uuid);
    printf("[connHandle] %d , [value handle] %d\n",connHandle,handle);

    //peripherals.insert(new value_type());
    if(uuid == UUID(GattCharacteristic::UUID_CURRENT_TIME_CHAR)){
        printf("**Current time Characteristic found\n");
        dev.time_handle = handle;   // store handle in object
        dev.is_CTS = true;          // peer is a device with CTS 
        // _CTC_handle = handle;
    }else if( uuid == UUID("F000aa71-0451-4000-B000-000000000000")){
        printf("**light char found.\n");
        dev.light_handle = handle;
        dev.is_beacon = true;       // peer is a beacon
    }
   
    // switch (uuid)
    // {
    // case GattCharacteristic::UUID_CURRENT_TIME_CHAR:
    //             printf("**Current time Characteristic found. Value handle %d\n",handle);
    //             _CTC_handle = handle;
    //     break;
    
    // default:
        
    //     break;
    // }
    
}


// Stop the discovery process and clean the instance.
void GattClientProcess::stop()
{
    if (!_gattClient) {
        printf("no gatt instance\n");
         return;
         }

    // unregister event handlers
    _gattClient->onServiceDiscoveryTermination(NULL);

    if(_ble_interface == NULL){
        printf("NO ble interface.\n");
        return;
    }

    //printf("Service discovery stopped. Disconnecting handle %d..",_connection_handle);

//     ble_error_t error = _ble_interface->gap().disconnect(
//    //     _connection_handle,
//         ble::local_disconnection_reason_t::USER_TERMINATION
//     );

    if(error){
     //   printf("\nError %d during disconnect handle %d\n",_connection_handle);
        return;
    }
    printf("OK\n");
    
    // Clear up instance
    //_gattClient = NULL;
    // _connection_handle = 0;
}

void GattClientProcess::setRTC(const uint8_t *p_data,uint16_t len){

        uint8_t data[len];
        memcpy(data,p_data,len);

        DateTime_t time;
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

        
        struct tm when = {0};

        when.tm_hour = time.hour;
        when.tm_min = time.minute;
        when.tm_sec = time.second;

        when.tm_year = time.year - 1900;
        when.tm_mon = time.month - 1; // month since janury
        when.tm_mday = time.day;
        when.tm_isdst = 1; //Daylight Saving Time flag effect

        char buffer[100];

        time_t convert = mktime(&when);  // t is now your desired time_t
        set_time(convert);

        // When finish setting time, disconnect
        printf("Finish reading target characteristic.\n");

        // empty buffer
        memset(buffer,0,sizeof(buffer));

        // get current time from system
        time_t _current_time = std::time(NULL);
        
        //printf("Time as a basic string = %s", ctime(&_current_time));
        // "H"hour(24h)|"M"minute(00-59)|"S"second(00-60)|"d"day(01-31)| "m"month |"Y"years| "u"day of week(1-7) | 
        strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S  day of week %u",localtime(&_current_time));
        printf("local time %s\r\n",buffer);

        // _event_queue->call(this,&Self::stop);
}

void GattClientProcess::print_uuid(const UUID &uuid)
{
    const uint8_t* p_uuid = uuid.getBaseUUID();
    uint8_t len = uuid.getLen();
    // uuid is LSB
    for(int i = len - 1; i >-1; i--){
            printf("%.2x ",p_uuid[i]);
    }
    printf("\n");
}


template<typename Arg>
FunctionPointerWithContext<Arg> GattClientProcess::as_cb(void (Self::*member)(Arg))
{
    return makeFunctionPointer(this, member);
}


void GattClientProcess::print_device_info(device_t &dev){
    printf("dev.connection_handle = %d\n",dev.connection_handle);
    printf("dev.is_CTS = %d\n",dev.is_CTS);
    printf("dev.is_beacon = %d\n",dev.is_beacon);
    const uint8_t *addr = dev.address;
    printf("dev.address = %02x:%02x:%02x:%02x:%02x:%02x\n",
           addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    printf("dev.time_handle = %d\n",dev.time_handle);
    printf("dev.light_handle = %d\n",dev.light_handle);
    printf("dev.motion_handle = %d\n",dev.motion_handle);
}

