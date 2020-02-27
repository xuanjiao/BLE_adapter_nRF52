
#include "mbed.h"

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)


I2C i2c(p26,p27); 
class MeasurementProcess{
    private:
        //mbed::Callback<void(sensor_type type,uint8_t,char*)> _post_update_value_cb[CALLBACK_NUM_MAX];
        mbed::Callback<void(uint8_t)> _post_update_cb;
     //   LDR &_ldr;

        //I2C i2c(I2C_SDA , I2C_SCL);
        char cmd[1];
        const int addr8bit = 0x46; // datasheet 0x23
        
   public:

        void init(){
            i2c.frequency(100000);

            wait(0.5);

            // cmd[1] = 0x01;
            cmd[0] = 0x10;
            int ret = i2c.write(addr8bit, cmd, 1,false);
                if(ret != 0){
                    printf("Error %d during write to 0x%x.\n",ret,addr8bit);
                }
            
            wait(0.5);
/*
             int ret;
                cmd[0]= 0x01;
                ret = i2c.write(addr8bit, cmd, 1,false);
                if(ret != 0){
                    printf("Error %d during write to 0x%x.\n",ret,addr8bit);
                }
*/
      //          wait_ms(240);

            /*
                int ack;

                // cmd[0] = 0x10;
                cmd[0] = 0x11;
                cmd[1] = 0x10;
                int ret = i2c.write(addr8bit, cmd, 2);
                if(ret != 0){
                    printf("Error %d during write to 0x%x.\n",ret,addr8bit);
                }
                
               
                wait(0.5);
                ack =  i2.read(1);
                printf("get a bit %d",ack);

                cmd[0] = 0x00;
                cmd[1] = 0x01
                */
        }

        void read() {
                int ret;
                // cmd[0]= 0x01;
                // ret = i2c.write(addr8bit, cmd, 1,false);
                // if(ret != 0){
                //     printf("Error %d during write to 0x%x.\n",ret,addr8bit);
                // }

                i2c.read( addr8bit, cmd, 2,true);
                
                
                if(ret != 0){
                    printf("Error %d during read from 0x%x.\n",ret,addr8bit);
                    return;
                }
                int high = cmd[0];
                int low = cmd[1];
                printf("%x %x\n",cmd[0],cmd[1]);

                int sum = high * 256 + low;
                printf("high %d low %d light= %d\n", high,low,sum);
        }
  //        MeasurementProcess(LDR &ldr):{}
    //   _ldr(ldr)
/*
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
        */
};
