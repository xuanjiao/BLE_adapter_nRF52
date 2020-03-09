#ifndef BLE_PROCESS_H
#define BLE_PROCESS_H

#include <events/mbed_events.h>
#include "Callback.h"
#include "BLE.h"
#include "gap/Gap.h"
#include "Gap.h"
#include "gap/AdvertisingDataParser.h"
#include "GapAdvertisingParams.h"
#include "gap/AdvertisingDataTypes.h"
//#include "GapAdvertisingData.h"
#include "FunctionPointerWithContext.h"
#include "GattClient.h"     
#include "SEGGER_RTT.h"
#include "UUID.h"
#include <mbed.h>
#include "util.h"
#include <Span.h>
#include <map>
#include "ble_define.h"


class BLEAdvScanProcess : public ble::Gap::EventHandler
{
    typedef BLEAdvScanProcess Self;

    typedef pair<ble::connection_handle_t, device_t> value_type;

private:

    events::EventQueue &_event_queue;

    BLE &_ble_interface;

    Gap &_gap;

    static const int CALLBACK_NUM_MAX = 3;

    bool _is_in_scanning_mode;

    bool _is_connecting;

    mbed::Callback<void(BLE&,events::EventQueue&)> _post_init_cbs[CALLBACK_NUM_MAX];

    mbed::Callback<void(device_t new_device)> _post_connection_compete_cb;
    
    int _count_init_cb;

    int _scan_count;

    int _on_duration_end_id;
    
    int _on_led_blinking_id;
    // InterruptIn *_button;

    // DigitalOut *_led1;

    bool start_adv;

    DigitalOut _led_scan;

    DigitalOut _led_adv;

    DigitalOut _led_connect;

   // InterruptIn _irq;
public:
    BLEAdvScanProcess(events::EventQueue &event_queue, BLE &ble_interface):
        _event_queue(event_queue),
        _ble_interface(ble_interface),
        _gap(ble_interface.gap()),
        _scan_count(0),
        _is_in_scanning_mode(true),
        _is_connecting(false),
        _on_duration_end_id(0),
        start_adv(false),
        _led_adv(LED1,1),
        _led_scan(LED2,1),
        _led_connect(LED3,1)
        {}

    ~BLEAdvScanProcess();
    
    void scan();

    void advertise();

    // Subscription to the ble interface initialization event.
    void on_init(mbed::Callback<void(BLE&,events::EventQueue&)> cb);

    // Subscription to the ble interface connection complete event.
    void on_connection_complete(mbed::Callback<void(device_t)> cb);

    
    // Initialize the ble interface, configure it and start advertising.
    bool start();

    // Close existing connection and stop process
    void stop();

    private:

    // Called when advertising ends.
    virtual void onAdvertisingEnd(const ble::AdvertisingEndEvent &event);

    // Called when scan times out.
    virtual void onScanTimeout(const ble::ScanTimeoutEvent &event);
    
    // Called when a scanner receives an advertising or a scan response packet.
    virtual void onAdvertisingReport(const ble::AdvertisingReportEvent &event);	

    // Called when connection attempt ends or an advertising device has been connected.
    virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &event);

    // Called when a connection has been disconnected.
    virtual void onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event);
    
    
    //This is called when BLE interface is initialised and starts the first mode,
    //    it sets up adverting payload and start advertising.
    void when_init_complete(BLE::InitializationCompleteCallbackContext *event);

    // Schedule processing of events from the BLE middleware in the event queue.
    void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *event);

    void connectToPeer(ble::peer_address_type_t peerAddressType,
                                            const ble::address_t & peerAddress,
                                            const ble::ConnectionParameters & connectionParams);
                                            
    void mode_start();

    void mode_end();

    void led_connect_change_state();

    void print_address(const uint8_t *addr);

    void print_uuid(const UUID &uuid);
};

#endif