#include "LDR.h"
#include "BLEAdvScanProcess.h"
#include "EnviromentSensingServer.h"
#include "sdcardProcess.h"
#include "sensor_type.h"
#include "GattClientProcess.h"
//#include "MeasurementProcess.h"
#include <stdint.h>

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

int main(){
    printf("Main function start.\n");

    BLE &ble_interface = BLE::Instance();          
    events::EventQueue event_queue;
    SDcardProcess sd(event_queue);

    sd.init_sd_card();

    BLEAdvScanProcess ble_process(event_queue,ble_interface);
    EnviromentSensingServer server;
    GattClientProcess client;

    // when ble initialize finish, initialize gatt server and gatt client.
    ble_process.on_init(
        mbed::callback(&server,&EnviromentSensingServer::start)
    );

    ble_process.on_init(
        mbed::callback(&client,&GattClientProcess::init)
    );
            
    // When a connected to a BLE periphral device, start service discovery. 
    ble_process.on_connection_complete(
         mbed::callback(&client,&GattClientProcess::start_service_discovery)
    );

    client.on_read_sensor_value_complete_cb(
        mbed::callback(&sd,&SDcardProcess::record_beacon)
    );
             
    // Bind the event queue to the ble interface, initialize the interface
    // and start advertising
    ble_process.start();
      
    event_queue.dispatch();

            return 0;
}