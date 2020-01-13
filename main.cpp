#include<LDR.h>
#include<ble_process/BLEProcess.h>
#include<acd52832_bsp.h>


#define UPDATE_DATA_INTERVAL            2000
#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

class Demo{
    private:
        BLE *_ble_interface;
        events::EventQueue *_event_queue;
        LDR *_ldr;
   public:

        Demo(LDR &ldr){
            _ldr = &ldr;
        }

        void init(BLE &ble_interface, events::EventQueue &event_queue)
        {
            _ble_interface = &ble_interface;
            _event_queue = &event_queue;
          //  _client = &_ble_interface->gattClient();

          //  _ble_interface->gap().setEventHandler(this);
            _event_queue->call_every(UPDATE_DATA_INTERVAL,this,&Demo::update_sensor_value);
        }

        void update_sensor_value(){
              // Value normalized to a byte [0,255]
            uint8_t uLight = _ldr->getLight();
            printf("light = %x\r\n",uLight);

        }

};
        int main(){
            BLE &ble_interface = BLE::Instance();
            events::EventQueue event_queue;
            LDR ldr(ADC_LIGHT);
            Demo demo(ldr);
            BLEProcess ble_process(event_queue,ble_interface);
            
            // Register GattClientProcess::init in the ble_process; this function will
            // be called once the ble_interface is initialized.
            ble_process.on_init(
                mbed::callback(&demo,&Demo::init)
            );

            // bind the event queue to the ble interface, initialize the interface
            // and start advertising
            ble_process.start();

            // Process the event queue.
            event_queue.dispatch_forever();

            return 0;
        }