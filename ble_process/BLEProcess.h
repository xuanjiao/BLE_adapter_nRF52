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

class BLEProcess : private mbed::NonCopyable<BLEProcess>
{
private:
    events::EventQueue &_event_queue;
    BLE &_ble_interface;
    mbed::Callback<void(BLE&)> _post_init_cb;
public:
    BLEProcess(events::EventQueue &event_queue, BLE &ble_interface):
        _event_queue(event_queue),
        _ble_interface(ble_interface),
        _post_init_cb(){
    }

    ~BLEProcess();
    
    

    // Subscripton_inition to the ble interface initialization event.
    void on_init(mbed::Callback<void(BLE&)> cb);

    // Initialize the ble interface, configure it and start advertising.
    bool start();

    // Close existing connection and stop process
    void stop();

    private:

    // Sets up adverting payload and start advertising.
    void when_init_complete(BLE::InitializationCompleteCallbackContext *event);

    // Start the gatt client process when a connection event is received.
    void when_connection(const Gap::ConnectionCallbackParams_t *connection_event);

    // Stop the gatt client process when the device is disconnected then restart
    // advertising.
     void when_disconnection(const Gap::DisconnectionCallbackParams_t *event);

     // Start the advertising process; it ends when a device connects.
    void start_advertising();

    // Schedule processing of events from the BLE middleware in the event queue.
    void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *event);
    
    // Build data advertised by the BLE interface.
    GapAdvertisingData make_advertising_data();

    GapAdvertisingParams make_advertising_params();

    // Build advertising parameters used by the BLE interface.


};

#endif