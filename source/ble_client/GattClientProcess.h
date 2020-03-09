#include "BLE.h"
#include "gap/Gap.h"
#include "Gap.h"
#include "GattClient.h"
#include "GattCharacteristic.h"
#include "GapAdvertisingParams.h"
#include "GapAdvertisingData.h"
#include "DiscoveredService.h"
#include "DiscoveredCharacteristic.h"
#include "CharacteristicDescriptorDiscovery.h"
#include "GattAttribute.h"
#include "ble_define.h"
#include "SEGGER_RTT.h"
#include "BLEAdvScanProcess.h"

class GattClientProcess : public ble::Gap::EventHandler{

    typedef GattClientProcess Self;

    typedef CharacteristicDescriptorDiscovery::DiscoveryCallbackParams_t DiscoveryCallbackParams_t;

    typedef CharacteristicDescriptorDiscovery::TerminationCallbackParams_t TerminationCallbackParams_t;

    typedef DiscoveredCharacteristic::Properties_t Properties_t;

    public:

        GattClientProcess():
            _gattClient(NULL),
            _event_queue(NULL),
            _ble_interface(NULL)
            // _CTC_handle(0),
            // _connection_handle(0)
            {

        }
        
        ~GattClientProcess()
        {
            stop_service_discovery();
        }
      
        void init(BLE &ble_interface, events::EventQueue &event_queue);

        // Launch the service and characteristic discovery procedure of a GATT server peer.
        void start_service_discovery(device_t new_device);

        void stop_service_discovery();

        void print_device_info(device_t &dev);

        void disconnect_peer(device_t &dev);
    private:
        /**
         * Helper that construct an event handler from a member function of this
         * instance.
         */
        template<typename Arg>
        FunctionPointerWithContext<Arg> as_cb(void (Self::*member)(Arg));

        void print_uuid(const UUID &uuid);

        void when_connection(const Gap::ConnectionCallbackParams_t *connection_event);

        // Service discovered event handler invoked when a matching service has been discovered. 
        void when_service_discovered(const DiscoveredService* discovered_service);

        // 	Characteristic discovered event handler invoked when a matching characteristic has been found. 
        void when_characteristic_discovered(const DiscoveredCharacteristic* discovered_characteristic);

        void when_service_discovery_ends(const ble::connection_handle_t connection_handle);

        // Attribute read event handler.
        void when_char_data_read(const GattReadCallbackParams* params);
        
        void read_value_all_sensors();

        void setRTC(const uint8_t *p_data,uint16_t len);
        
        void print_addr(const uint8_t *addr);

        GattClient *_gattClient;

        events::EventQueue *_event_queue;

        BLE *_ble_interface;
        
        std::map<ble::connection_handle_t,device_t> devices;

};