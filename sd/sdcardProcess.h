#include "mbed.h"
#include <stdio.h>
#include <errno.h>
#include "SDBlockDevice.h"
#include "BlockDevice.h"
#include "FATFileSystem.h"
#include "SEGGER_RTT.h"

class SDcardProcess{
    private:
        //SDBlockDevice &_sd;
        BlockDevice* _bd;
        FATFileSystem fs;
        const static int frequency = 5000000;
        char buffer[512];
        const char* log_file_name ="light_log.txt";

    public:
   //     SDcardProcess(SDBlockDevice &sd):_sd(sd){}
        SDcardProcess(){
            _bd = BlockDevice::get_default_instance();
        }
        void print_sd_card_info();
        bool erase_sd_card();
        bool init_sd_card();
        bool write_sensor_value_and_time(int value,char* time);
        bool open_log_file();
};
