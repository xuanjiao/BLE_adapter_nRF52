#include "LDR.h"
#include <ble_process/BLEProcess.h>
#include <ble_process/LDRService.h>
#include "sdcardProcess.h"


#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

class MeasurementProcess{
    private:
        const static int CALLBACK_NUM_MAX = 3;
        mbed::Callback<void(uint8_t,char*)> _post_update_value_cb[CALLBACK_NUM_MAX];
        LDR &_ldr;
        int _callback_num;
        
        
   public:
        MeasurementProcess(LDR &ldr):
        _ldr(ldr),_callback_num(0){}

        // Store callbacks in callback array.
        void registerCallback(mbed::Callback<void(uint8_t,char*)> cb){           
            if(cb){
                printf("register callback no.%d\r\n",_callback_num+1);
                if(_callback_num < CALLBACK_NUM_MAX)
                {
                    _post_update_value_cb[_callback_num] = cb;
                    _callback_num++;
                }else{
                    printf("Max. callback number is %d\n",CALLBACK_NUM_MAX);
                }
            }       
        }

        // Get current light value from light sensor and give it to ble module
        void measureLight(){
            uint8_t light = _ldr.getLight();
            
            time_t _current_time = time(NULL);
            
            char buffer[50];
            // "H"hour(24h)|"M"minute(00-59)|"S"second(00-60)|"d"day(01-31)| "m"month |"Y"years| "u"day of week(1-7) | 
            strftime(buffer,sizeof(buffer),"%H:%M:%S %d-%m-%Y day of week %u",localtime(&_current_time));
            printf("give light value %x, time %s\r\n",light,buffer);

            // Here run LDRService::update_sensor_value(uint8_t light).
            for(int i = 0; i < _callback_num;i++)
            {
                _post_update_value_cb[i](light,buffer);  
            }
                    
        }
};
        
        int main(){
            BLE &ble_interface = BLE::Instance();
            LDR ldr(MBED_CONF_APP_PIN_LIGHT);
            SDcardProcess sd;
            sd.init_sd_card();

            sd.write_sensor_value_and_time(30,"2020-01-20");
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

            measurement_process.registerCallback(
                mbed::callback(&ldr_service,&LDRService::update_sensor_value)
            );
            
            event_queue.call_every(MBED_CONF_APP_MEASUREMENT_INTERVAL,
                                    &measurement_process,
                                    &MeasurementProcess::measureLight);

            // Process the event queue.
            event_queue.dispatch();

            return 0;
        }