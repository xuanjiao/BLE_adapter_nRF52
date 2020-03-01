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
//#include <mbed.h>
#include "util.h"
#include <Span.h>

typedef struct {
    ble::advertising_type_t type;
    ble::adv_interval_t min_interval;
    ble::adv_interval_t max_interval;       
} AdvParams_t;

typedef struct {
    ble::scan_interval_t interval;
    ble::scan_window_t window;
    ble::scan_duration_t duration;
    bool active;
} ScanParams_t;

typedef struct {
    UUID uuid;
    uint8_t value;
} AdvServiceData_t;

static const AdvParams_t adv_params = {
    ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
    ble::adv_interval_t(40),    // 0.625us x25 = 40us
    ble::adv_interval_t(80)     //  80 us
};

static const ScanParams_t scan_params = {
    ble::scan_interval_t(160),  // 0.625us x  160 = 100us
    ble::scan_window_t(100),    //  625us x 100 = 6.25ms
    ble::scan_duration_t(0),     // scan never ends 
    true    // active scanning flag.
};

class BLEAdvScanProcess : public ble::Gap::EventHandler
{

private:
    static const uint16_t MAX_ADVERTISING_PAYLOAD_SIZE = 251;

    events::EventQueue &_event_queue;

    BLE &_ble_interface;

    Gap &_gap;

    const static int CALLBACK_NUM_MAX = 3;

    mbed::Callback<void(BLE&,events::EventQueue&)> _post_init_cbs[CALLBACK_NUM_MAX];

    mbed::Callback<void()> _post_connection_compete_cb;
    
    int _count_init_cb;

    int _scan_count;
    
public:
    BLEAdvScanProcess(events::EventQueue &event_queue, BLE &ble_interface):
        _event_queue(event_queue),
        _ble_interface(ble_interface),
        _gap(ble_interface.gap()),
        _scan_count(0)
        {}

    ~BLEAdvScanProcess();
    
    void scan();

    void advertise();

    // Subscription to the ble interface initialization event.
    void on_init(mbed::Callback<void(BLE&,events::EventQueue&)> cb);

    // Subscription to the ble interface connection complete event.
    void on_connection_complete(mbed::Callback<void()> cb);

    
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

    /** This is called when BLE interface is initialised and starts the first mode,
        it sets up adverting payload and start advertising.
    **/
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

    void print_address(const uint8_t *addr);
};

#endif