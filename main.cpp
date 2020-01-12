#include<ble_process/BLEProcess.h>


class GattProcess{
    private:
        BLE *_ble_interface;
        events::EventQueue *_event_queue;
   public:
        void init(BLE &ble_interface, events::EventQueue &event_queue)
        {
            _ble_interface = &ble_interface;
            _event_queue = &event_queue;
          //  _client = &_ble_interface->gattClient();

          //  _ble_interface->gap().setEventHandler(this);
        }

};
        int main(){
            BLE &ble_interface = BLE::Instance();
            events::EventQueue event_queue;
            GattProcess gattProcess;
            BLEProcess ble_process(event_queue,ble_interface);
            
            // Register GattClientProcess::init in the ble_process; this function will
            // be called once the ble_interface is initialized.
            ble_process.on_init(
                mbed::callback(&gattProcess,&GattProcess::init)
            );

            // bind the event queue to the ble interface, initialize the interface
            // and start advertising
            ble_process.start();

            // Process the event queue.
            event_queue.dispatch_forever();

            return 0;
        }