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
#include "ble_define.h"

using namespace std;

static const char* FILE_SYSTEM_NAME = "fs";
      
// log file stores in file system        

class SDcardProcess{
    typedef SDcardProcess Self;

    public:

        // Get a gefault Block device(SD card) object and create a file system object for it.
        // The SPI connection of sd card can be configure in mbed_app.json file.
        SDcardProcess(
            events::EventQueue &event_queue
        ):  _file_system(FILE_SYSTEM_NAME),
            _erase_button(BUTTON2),
            _ready_to_write(false),
            _event_queue(&event_queue),
            _block_device(NULL)
        {
            // Use button to reformat sd card.
            _erase_button.fall(
                event_queue.event(this,&Self::reformat_sd_card)
            );
        }

        SDcardProcess();

        ~SDcardProcess()
        {
            delete _block_device;
            // delete _fs;
            // delete _bd;
            // delete _irq;
        }

        void record_beacon(Device_t &dev);
        
        void reformat_sd_card();
        
        // Pint sd card info: size, read size, program size and eraze size.
        void print_sd_card_info();

        // Mount the file system. Reformat FAT file system if no file systen found.
        void init_sd_card();

       // Unmount sd card.
        void unmound_sd_card();

        // Display file name and file type in root system.
        void display_root_directory();

    private:
        bool _ready_to_write;

        events::EventQueue* _event_queue;

        FATFileSystem _file_system;

        BlockDevice *_block_device;

        InterruptIn _erase_button;  
        
};
