#include "mbed.h"
#include <stdio.h>
#include <errno.h>
#include "SDBlockDevice.h"
#include "BlockDevice.h"
#include "FATFileSystem.h"
#include "SEGGER_RTT.h"
#include "events/EventQueue.h"
#include "sensor_type.h"
#include <vector>

using namespace std;


// log file stores in file system        
typedef struct log_file
{
    Sensor_type type;
    int index;
    char path[50];
}log_file;

class SDcardProcess{
    typedef SDcardProcess Self;

    public:

        // Get a gefault Block device(SD card) object and create a file system object for it.
        // The SPI connection of sd card can be configure in mbed_app.json file.
        SDcardProcess(events::EventQueue &event_queue);

        SDcardProcess();

        ~SDcardProcess()
        {
            delete _fs;
            delete _bd;
            delete _irq;
        }
        
        void add_log_file(Sensor_type type,int index);
        
        // Pint sd card info: size, read size, program size and eraze size.
        void print_sd_card_info();

        bool remove_all_log_file();

        // Mount the file system. Reformat FAT file system if no file systen found.
        bool init_sd_card();

        // Write value and timestamp to log file
        void write_sensor_value_and_time(Sensor_type type,uint8_t value,char* time);
        
        // Display all data in log file
        // log file in in root directory of sd card.
        // file name can be difine in mbed_app.json file
        bool read_log_file();

        // Unmount sd card.
        bool close_sd_card();

        // Display file name and file type in root system.
        bool display_root_directory();

    private:
        FILE* open_file(const char *file_path, const char *mode);

        void set_current_log_file(log_file &file);
        
        events::EventQueue *_event_queue;

        BlockDevice *_bd;
        
        FATFileSystem *_fs;
        
        bool close_file(FILE *fp);
        
        const static int PATH_LEN_MAX = 50;
        
        const static int FILE_NUM_MAX = 5;

        
        InterruptIn *_irq;    
        // Open file and return a file pointer.
        
     
        vector<log_file> _log_files;

        log_file _my_log;

};
