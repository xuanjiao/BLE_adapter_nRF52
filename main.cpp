#include "LDR.h"
#include <ble_process/BLEProcess.h>
#include <ble_process/LDRService.h>
#include "sdcardProcess.h"


#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

class MeasurementProcess{
    private:
        mbed::Callback<void(uint8_t)> _post_update_cb;
        LDR &_ldr;
        
   public:
        MeasurementProcess(LDR &ldr):
        _ldr(ldr){}

        void registerCallback(mbed::Callback<void(uint8_t)> cb){           
            if(cb){
                printf("register callback\r\n");
                _post_update_cb = cb;
            }       
        }

        // Get current light value from light sensor and give it to ble module
        void measureLight(){
            uint8_t light = _ldr.getLight();
            printf("give light value %x\r\n",light);

            // Here run LDRService::update_sensor_value(uint8_t light).
            _post_update_cb(light);          
        }
};
        
        int main(){
            BLE &ble_interface = BLE::Instance();
            LDR ldr(PIN_LIGHT);
            SDcardProcess sd;
            sd.init_sd_card();
            sd.display_directory();
            sd.write_sensor_value_and_time(15,"2020");
            sd.close_sd_card();

            events::EventQueue event_queue;

            BLEProcess ble_process(event_queue,ble_interface);
            LDRService ldr_service;

            // Register LDRService::start in the ble_process; this function will
            // be called once the ble_interface is initialized.
            ble_process.on_init(
                mbed::callback(&ldr_service,&LDRService::start)
            );

            // bind the event queue to the ble interface, initialize the interface
            // and start advertising
            ble_process.start();

            MeasurementProcess measurement_process(ldr);

            // Register LDRService::update_sensor_value in the measurement_process, 
            // this function will be called after measuring light value
            measurement_process.registerCallback(
                mbed::callback(&ldr_service,&LDRService::update_sensor_value)
            );
            
            event_queue.call_every(MEASURMENT_INTERAL,&measurement_process,&MeasurementProcess::measureLight);

            // Process the event queue.
            event_queue.dispatch();

            return 0;
        }