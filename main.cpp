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
        //mbed::Callback<void(LDR&)> _post_update_cb;
        mbed::Callback<void(uint8_t)> _post_update_cb;
        
        
   public:

        Demo(){
        
        }
        void registerCallback(mbed::Callback<void(uint8_t)> cb){           
            if(cb){
                printf("register callback\r\n");
                _post_update_cb = cb;
            }
            
        }


        void measureLight(){
            LDR ldr(ADC_LIGHT);
            uint8_t light = ldr.getLight();
            printf("give light value %x\r\n",light);
            _post_update_cb(light);          
        }
};
        
        int main(){
            BLE &ble_interface = BLE::Instance();
            events::EventQueue event_queue;

            BLEProcess ble_process(event_queue,ble_interface);
            LDRService ldr_service;

            // Register GattClientProcess::init in the ble_process; this function will
            // be called once the ble_interface is initialized.
            ble_process.on_init(
                mbed::callback(&ldr_service,&LDRService::start)
            );

            Demo demo;
            demo.registerCallback(
                mbed::callback(&ldr_service,&LDRService::update_sensor_value)
            );
            
            
            int id = event_queue.call_every(2000,&demo,&Demo::measureLight);
            printf("call every 1000ms, id = %d\r\n",id);

            // bind the event queue to the ble interface, initialize the interface
            // and start advertising
            ble_process.start();

            

            // Process the event queue.
            event_queue.dispatch();

            return 0;
        }