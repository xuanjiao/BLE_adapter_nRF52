#include "mbed.h"
#include <stdio.h>
#include <errno.h>
#include "SDBlockDevice.h"
#include "BlockDevice.h"
#include "FATFileSystem.h"
#include "SEGGER_RTT.h"


class SDcardProcess{
    public:

        // Get a gefault Block device(SD card) object and create a file system object for it.
        // The SPI connection of sd card can be configure in mbed_app.json file.
        SDcardProcess();

        ~SDcardProcess()
        {
            delete _fs;
            delete _bd;
            delete _irq;
        }

        // Pint sd card info: size, read size, program size and eraze size.

        // Size
        void print_sd_card_info();

        bool erase_sd_card();

        // Mount the file system. Reformat FAT file system if no file systen found.
        bool init_sd_card();

        // Write value and timestamp to log file
        bool write_sensor_value_and_time(int value,char *time);
        
        // Display all data in log file
        // log file in in root directory of sd card.
        // file name can be difine in mbed_app.json file
        bool read_log_file();

        // Unmount sd card.
        bool close_sd_card();

        // Display file name and file type in root system.
        bool display_root_directory();

            private:
        //SDBlockDevice &_sd;
        BlockDevice *_bd;
        FATFileSystem *_fs;
        bool close_file(FILE *fp);
        const static int PATH_LEN_MAX = 50;
        char file_path[PATH_LEN_MAX];
        InterruptIn *_irq;    
        // Open file and return a file pointer.
        FILE* open_file(const char *file_path, const char *mode);


};
