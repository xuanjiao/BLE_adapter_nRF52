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
        BlockDevice *_bd;
        FATFileSystem *_fs;
        bool close_file(FILE *fp);
        char path[50];

    public:

        SDcardProcess();

        ~SDcardProcess()
        {
            delete _fs;
            delete _bd;
        }

        void print_sd_card_info();
        bool erase_sd_card();
        bool init_sd_card();
        bool write_sensor_value_and_time(int value,char* time);
        bool read_log_file();
        bool close_sd_card();
        FILE* open_file(char *path);
        void display_directory();
        
        
};
