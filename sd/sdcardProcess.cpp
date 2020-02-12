
#include "sdcardProcess.h"

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

void SDcardProcess::print_bd_card_info(){
    printf("sd size: %llu\n",         _bd.size());
    printf("sd read size: %llu\n",    _bd.get_read_size());
    printf("sd program size: %llu\n", _bd.get_program_size());
    printf("sd erase size: %llu\n",   _bd.get_erase_size());
}

bool SDcardProcess::erase_sd_card(){
    int ret;
    if( ( ret = _bd.erase(0,_bd.get_erase_size())) != 0){
        printf("Error %d during eraseiing block\n");
        return false;
    }
        return true;
}
        
bool SDcardProcess::init_sd_card(){
            /*
            int ret;
            if( (ret = _bd.init())!=0){
                printf("Error %d during init sd card\n");
                return false;
            }

            // Set the frequency
            if( (ret = _sd.frequency(frequency)) != 0 ){
                printf("Error %d during setting frequency.\n");
            } 
            */

            // Mount the filesystem
    printf("Mounting the filesystem...");
    fflush(stdout);
    if((ret = fs.mount(&_bd))!=0){
        printf("Error %d during mounting the filesystem\n");
        printf("No system found, formating...");
        fflush(stdout);

        if((ret = fs.reformat(&_bd))!=0){
            printf("Error %d during reformat the sd card.\n");
            return false;
         }
    }else{
        printf("Mounting file system succeed.\n");
    }

    return true;

}

bool SDcardProcess::write_sensor_value_and_time(int value,char* time){
    int ret,n;
    n = sprintf(buffer,"%d %s",value,time);
            /*
            if( (ret = _sd.program(buffer,0,sizeof(buffer)))!=0){
                printf("Error %d during write data block to sd card.\n");
                return false;
            }else{
                printf("Wrote %d byte data to sd card.\n");
            }
            */
    printf("Opening \"/fs/%d\"...\n",log_file_name);
    char path[30];
    FILE* f = fopen("/fs/");
}

bool SDcardProcess::open_log_file(){

}