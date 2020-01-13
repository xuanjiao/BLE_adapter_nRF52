#ifndef BLE_PROCESS_H
#define BLE_PROCESS_H

#include "events/EventQueue.h"
#include "platform/Callback.h"
#include "platform/NonCopyable.h"
 
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ble/GapAdvertisingParams.h"
#include "ble/GapAdvertisingData.h"
#include "ble/FunctionPointerWithContext.h"
#include "ble/GattClient.h"

#include "SEGGER_RTT.h"

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

#define DEVICE_NAME     "ACD52"

class BLEProcess : private mbed::NonCopyable<BLEProcess>
{
private:
    events::EventQueue &_event_queue;
    BLE &_ble_interface;
    mbed::Callback<void(BLE&, events::EventQueue&)> _post_init_cb;
public:
    BLEProcess(events::EventQueue &event_queue, BLE &ble_interface):
        _event_queue(event_queue),
        _ble_interface(ble_interface),
        _post_init_cb(){
    }
    
    ~BLEProcess()
    {
        stop();
    }

    // Subscription to the ble interface initialization event.
    void on_init(mbed::Callback<void(BLE&, events::EventQueue&)> cb)
    {
        _post_init_cb = cb;
    }
 

    // Initialize the ble interface, configure it and start advertising.
    bool start()
    {
        printf("Ble process start\n");

        if(_ble_interface.hasInitialized()){
            printf("Error: the ble instance has already been initialized.\n");
            return false;
        }

        // It will be called every time the BLE stack has pending work. 
        _ble_interface.onEventsToProcess(
            makeFunctionPointer(this,&BLEProcess::schedule_ble_events)
        );
        
        ble_error_t error = _ble_interface.init(this,&BLEProcess::when_init_complete);
        if(error){
            printf("Error: %u returned by BLE::init.\r\n", error);
            return false;
        }
        return true;
    }

    // Close existing connection and stop process
    void stop()
    {
        if(_ble_interface.hasInitialized()){
            _ble_interface.shutdown();
            printf("BLE process stopped");
        }
    }

    private:

    // Sets up adverting payload and start advertising.
    void when_init_complete(BLE::InitializationCompleteCallbackContext *event)
    {
        if(event->error){
            printf("Error %u during the initialization\r\n", event->error);
            return;
        }
        printf("BLE instance initialized.\r\n");

        Gap &gap = _ble_interface.gap();
        ble_error_t error = gap.setAdvertisingPayload(make_advertising_data());
        if(error){
            printf("Error %u during gap.setAdvertisingPayload");
        }

        gap.setAdvertisingParams(make_advertising_params());

        gap.onConnection(this, &BLEProcess::when_connection);
        gap.onDisconnection(this, &BLEProcess::when_disconnection);

        start_advertising();

        if (_post_init_cb) {
            _post_init_cb(_ble_interface, _event_queue);
        }

    }

    // Start the gatt client process when a connection event is received.
     void when_connection(const Gap::ConnectionCallbackParams_t *connection_event)
     {
         printf("Connected. \r\n");
     }

    // Stop the gatt client process when the device is disconnected then restart
    // advertising.
     void when_disconnection(const Gap::DisconnectionCallbackParams_t *event)
     {
         printf("Disconnected.\r\n");
         start_advertising();
     }

     // Start the advertising process; it ends when a device connects.
    void start_advertising()
    {
        ble_error_t error = _ble_interface.gap().startAdvertising();
        if(error){
            printf("Error %u during gap.startAdvertising.\r\n");
        }else{
            printf("Advertising start.\r\n");
        }
    }

    // Schedule processing of events from the BLE middleware in the event queue.
    void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *event)
    {
        _event_queue.call(mbed::callback(&event->ble, &BLE::processEvents));
    }

    // Build data advertised by the BLE interface.
    static GapAdvertisingData make_advertising_data(){
        GapAdvertisingData advertising_data;

        // add advertising flags
        advertising_data.addFlags(
            GapAdvertisingData::LE_GENERAL_DISCOVERABLE | // Peripheral device is discoverable at any moment.
            GapAdvertisingData::BREDR_NOT_SUPPORTED     // Peripheral device is LE only.
        );

        // add device name
        advertising_data.addData(
            GapAdvertisingData::COMPLETE_LOCAL_NAME, 
            (uint8_t *)DEVICE_NAME, 
            strlen(DEVICE_NAME)
        );

        return advertising_data;
    }

    static GapAdvertisingParams make_advertising_params(){
        return GapAdvertisingParams(
            /* type */     GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED,
            /* interval */ GapAdvertisingParams::MSEC_TO_ADVERTISEMENT_DURATION_UNITS(500),
            /* timeout */  0
        );
    }

    // Build advertising parameters used by the BLE interface.


};

#endif