#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ble/GattClient.h"
#include "ble/GattCharacteristic.h"
#include "ble/GapAdvertisingParams.h"
#include "ble/GapAdvertisingData.h"
#include "ble/GattClient.h"
#include "ble/DiscoveredService.h"
#include "ble/DiscoveredCharacteristic.h"
#include "ble/CharacteristicDescriptorDiscovery.h"
#include "ble/GattAttribute.h"
#include <GattCharacteristic.h>

#include "SEGGER_RTT.h"
#include "BLEProcess.h"

class GattClientProcess : public ble::Gap::EventHandler{

    typedef GattClientProcess Self;

    typedef CharacteristicDescriptorDiscovery::DiscoveryCallbackParams_t DiscoveryCallbackParams_t;

    typedef CharacteristicDescriptorDiscovery::TerminationCallbackParams_t TerminationCallbackParams_t;

    typedef DiscoveredCharacteristic::Properties_t Properties_t;

    public:

        GattClientProcess():
            _gattClient(NULL),
            _event_queue(NULL),
            _ble_interface(NULL),
            _CTC_handle(0),
            _connection_handle(0){

        }
        
        ~GattClientProcess()
        {
            stop();
        }
      
        void init(BLE &ble_interface, events::EventQueue &event_queue);

        // Launch the service and characteristic discovery procedure of a GATT server peer.
        void start();

        void stop();

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

        void when_service_discovery_ends(ble::connection_handle_t connection_handle);

        // Attribute read event handler.
        void when_char_data_read(const GattReadCallbackParams* params);
        
        // Called when connection attempt ends or an advertising device has been connected.
        virtual void onConnectionComplete (const ble::ConnectionCompleteEvent &event);
        
        GattClient *_gattClient;

        events::EventQueue *_event_queue;

        BLE *_ble_interface;

        GattAttribute::Handle_t _CTC_handle;

        // Handle of the connection with the peer GATT server.
        ble::connection_handle_t _connection_handle;

};