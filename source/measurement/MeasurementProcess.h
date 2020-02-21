
#include "mbed.h"

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

I2C i2c(p29,p30); 
class MeasurementProcess{
    private:
        mbed::Callback<void(uint8_t)> _post_update_cb;
     //   LDR &_ldr;

        //I2C i2c(I2C_SDA , I2C_SCL);
        char cmd[2];
        const int addr8bit = 0x23;
        
   public:

        void BH1750_Init(){
                
                cmd[0] = 0x10;
                i2c.write(addr8bit, cmd, 1);
 
                wait(0.5);
        }

        int BH1750_Read() {
                i2c.read( addr8bit, cmd, 2);
                float tmp = (float((cmd[0]<<8)|cmd[1]) / 256.0);
                printf("Temp = %.2f\n", tmp);
        }
  //        MeasurementProcess(LDR &ldr):{}
    //   _ldr(ldr)

        void registerCallback(mbed::Callback<void(uint8_t)> cb){           
            if(cb){
                printf("register callback\r\n");
                _post_update_cb = cb;
            }       
        }

        // Get current light value from light sensor and give it to ble module
        void measureLight(){
        //  uint8_t light = _ldr.getLight();
            uint8_t light = 15;
            printf("give light value %x\r\n",light);

            // Here run EnviromentSensingServer::update_sensor_value(uint8_t light).
            _post_update_cb(light);          
        }
};
