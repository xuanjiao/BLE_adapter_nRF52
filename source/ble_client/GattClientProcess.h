#include "ble_define.h"
#include "global.h"
#include "SEGGER_RTT.h"
#include "BLEAdvScanProcess.h"
#include "Callback.h"
#include <map>

static Sensor_char_uuid_t sensor_char_uuid_list[] = {
        // sensor type          sensor data uuid                            sensor config uuid
     {Sensor_type::light    ,UUID("F000aa71-0451-4000-B000-000000000000"), UUID("F000aa72-0451-4000-B000-000000000000")},
     {Sensor_type::magnetometer ,UUID("F000aa81-0451-4000-B000-000000000000"), UUID("F000aa82-0451-4000-B000-000000000000")},
     
};

static Sensor_cmd_t sensor_cmd_list[] = {
    {Sensor_type::light     , {0x01}        ,1},
    {Sensor_type::magnetometer  , {0x40, 0x00}  ,2}
};

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
            _count_cb(0)
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
        void start_service_discovery(const ble::ConnectionCompleteEvent& event);

        void stop_service_discovery();

        void on_read_sensor_value_complete_cb(mbed::Callback<void(Device_t&)> cb);

        void print_device_info(Device_t &dev);

        void disconnect_peer(Device_t &dev);
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
        
        void read_data_from_handle(ble::connection_handle_t connection_handle, GattAttribute::Handle_t data_handle);

        void setRTC(const uint8_t *p_data,uint16_t len);
        
        // deal with light value and store them in buffer
        void process_light_sensor_data(const uint8_t *p_data, Sensor_char_t* p_char);

        void process_magnetometer_sensor_data(const uint8_t *p_data, Sensor_char_t* p_char);

        void send_command_to_beacon(Device_t &dev);

        void print_addr(const uint8_t *addr);

        mbed::Callback<void(Device_t&)> _post_read_sensor_value_complete_cb[2];
        
        int _count_cb;

        GattClient *_gattClient;

        events::EventQueue *_event_queue;

        BLE *_ble_interface;
        
        std::map<ble::connection_handle_t,Device_t> devices;

        // period of requrest measurement result.
        static const int PERIOD_READ_BEACON_DATA = 10000;

        static const int CALLBACK_NUM_MAX = 2;
};