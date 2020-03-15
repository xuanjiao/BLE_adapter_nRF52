#include "LDR.h"
#include "BLEAdvScanProcess.h"
#include "EnviromentSensingServer.h"
#include "sdcardProcess.h"
#include "sensor_type.h"
#include "GattClientProcess.h"
//#include "MeasurementProcess.h"
#include <stdint.h>

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)


class MeasurementProcess{
    private:
        const static int CALLBACK_NUM_MAX = 3;
        mbed::Callback<void(Sensor_type type,uint8_t,char*)> _post_update_value_cb[CALLBACK_NUM_MAX];
        LDR &_ldr;
        int _callback_num;
        
        
   public:
        MeasurementProcess(LDR &ldr):
        _ldr(ldr),_callback_num(0){}

        // Store callbacks in callback array.
        void registerCallback(mbed::Callback<void(Sensor_type type,uint8_t,char*)> cb){           
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
            uint8_t value = _ldr.getLight();
            
            time_t _current_time = time(NULL);
            
            Sensor_type type = light;

            char buffer[50];
            // "H"hour(24h)|"M"minute(00-59)|"S"second(00-60)|"d"day(01-31)| "m"month |"Y"years| "u"day of week(1-7) | 
            strftime(buffer,sizeof(buffer),"%H:%M:%S %d-%m-%Y day of week %u",localtime(&_current_time));
            
            printf("give light value %x, time %s\r\n",value,buffer);

            // Here run EnviromentSensingServer::update_sensor_value(uint8_t light).
            for(int i = 0; i < _callback_num;i++)
            {
                _post_update_value_cb[i](type,value,buffer);  
            }
                    
        }
};

        
        int main(){
            printf("Main function start.\n");
            BLE &ble_interface = BLE::Instance();
            
            events::EventQueue event_queue;

           LDR ldr(MBED_CONF_APP_PIN_LIGHT);

            SDcardProcess sd(event_queue);
            sd.init_sd_card();
            sd.add_log_file(light,1);


            BLEAdvScanProcess ble_process(event_queue,ble_interface);
            EnviromentSensingServer server;
            GattClientProcess client;

            // // Register callbacks to the ble init event; this function will
            // // be called once the ble interface is initialized.
            ble_process.on_init(
                mbed::callback(&server,&EnviromentSensingServer::start)
            );

            ble_process.on_init(
                mbed::callback(&client,&GattClientProcess::init)
            );
            
            // Register callbacks in the ble onConnectionComplete event; this function will
            // be called once connected.
       
            ble_process.on_connection_complete(
                mbed::callback(&client,&GattClientProcess::start_service_discovery)
            );
        
            MeasurementProcess measurement_process(ldr);
           //MeasurementProcess measurement_process;
           // measurement_process.init();
            // measurement_process.read()
            // Register EnviromentSensingServer::update_sensor_value in the measurement_process, 
            // this function will be called after measuring light value
        
            measurement_process.registerCallback(
                mbed::callback(&server,&EnviromentSensingServer::update_sensor_value)
            );

             measurement_process.registerCallback(
                mbed::callback(&sd,&SDcardProcess::write_sensor_value_and_time)
            );
        
            // bind the event queue to the ble interface, initialize the interface
            // and start advertising
           ble_process.start();
      
      
            // event_queue.call_every(MBED_CONF_APP_MEASUREMENT_INTERVAL,
            //                         &measurement_process,
            //                         &MeasurementProcess::measureLight);

            // Process the event queue.
            event_queue.dispatch();

            return 0;
        }