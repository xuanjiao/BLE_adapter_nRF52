#include<LDR.h>
#include<ble_process/BLEProcess.h>
#include<ble_process/LDRService.h>
#include<acd52832_bsp.h>

#define UPDATE_DATA_INTERVAL            2000
#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

// Set up the button S1 to trigger an erase

class Demo{
    private:
      //  BLE _ble_interface;
        events::EventQueue _event_queue;
        LDR _ldr;
   public:

        Demo(LDR &ldr, events::EventQueue &event_queue):
            _ldr(ldr),
            _event_queue(event_queue)
         //   _ble_interface(ble_interface)
        {
            _event_queue.call_every(UPDATE_DATA_INTERVAL,this,&Demo::update_sensor_value);
        }

        void update_sensor_value(){
              // Value normalized to a byte [0,255]
            uint8_t uLight = _ldr.getLight();
            printf("light = %x\r\n",uLight);
        }

};
        int main(){
            BLE &ble_interface = BLE::Instance();
            events::EventQueue event_queue;
            LDR ldr(ADC_LIGHT);
            
            //Demo demo(ble_interface,ldr,event_queue);
            Demo demo(ldr,event_queue);

            BLEProcess ble_process(event_queue,ble_interface);
            LDRService ldr_service;

            // Register GattClientProcess::init in the ble_process; this function will
            // be called once the ble_interface is initialized.
            ble_process.on_init(
                mbed::callback(&ldr_service,&LDRService::start)
            );

            // bind the event queue to the ble interface, initialize the interface
            // and start advertising
            ble_process.start();

            // Process the event queue.
            event_queue.dispatch_forever();

            return 0;
        }