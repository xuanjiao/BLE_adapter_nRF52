
#include "sdcardProcess.h"

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

SDcardProcess::SDcardProcess()
{
    _bd = BlockDevice::get_default_instance();
    _fs = new FATFileSystem(MBED_CONF_APP_FILE_SYSTEM_NAME,_bd);
    sprintf(path,"/%s/%s",MBED_CONF_APP_FILE_SYSTEM_NAME,MBED_CONF_APP_LOG_FILE_NAME);
}

void SDcardProcess::print_sd_card_info(){
    printf("sd size: %llu\n",         _bd->size());
    printf("sd read size: %llu\n",    _bd->get_read_size());
    printf("sd program size: %llu\n", _bd->get_program_size());
    printf("sd erase size: %llu\n",   _bd->get_erase_size());
}

bool SDcardProcess::erase_sd_card(){
    int ret;
    if( ( ret = _bd->erase(0,_bd->get_erase_size())) != 0){
        printf("Error %d during eraseiing block\n");
        return false;
    }
        return true;
}
        
bool SDcardProcess::init_sd_card(){
           
            int ret;
            /*
            if( (ret = _bd->init())!=0){
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
    if((ret = _fs->mount(_bd))!=0){
        printf("Error %d during mounting the filesystem\n");
        printf("No system found, formating...");
        fflush(stdout);

        if((ret = _fs->reformat(_bd))!=0){
            printf("Error %d during reformat the sd card.\n");
            return false;
         }
    }
    
    printf("OK.\n");

    return true;

}

bool SDcardProcess::write_sensor_value_and_time(int value,char* time){
    int ret,n;
    FILE *fp;
    char buffer[512];
    
    // Construct a line of data: value time
    n = sprintf(buffer,"%d %s",value,time);

            /*
            if( (ret = _sd.program(buffer,0,sizeof(buffer)))!=0){
                printf("Error %d during write data block to sd card.\n");
                return false;
            }else{
                printf("Wrote %d byte data to sd card.\n");
            }
            */
    printf("Opening \"%s\"...",path);
    fflush(stdout);

    if( (fp = fopen(path,"w+"))==NULL)
    {
        printf("Failed\n");
        printf("Error %d during open file:  %s.\n",-errno,strerror(errno));
        return false;
    }

    printf("Writing data: %s...",buffer);
    
    // Clear output buffer
    fflush(stdout);

    if( (n = fprintf(fp,buffer))<0)
    {
        printf("Failed\nError %d during write data to file: %s.\n",-errno,strerror(errno));
        return false;
    }
    
    printf("OK. wrote %d bytes\n",n);

    return true;
}

bool SDcardProcess::close_sd_card(){
    int ret;
    
    printf("Unmounting... ");

    if( (ret = _fs->unmount())<0)
    {
        printf("Failed\nError %d during unmount: %s.\n",-errno,strerror(errno));
        return false;
    }
    
    printf("OK.\n");

    return true;
}

bool SDcardProcess::read_log_file(){
    int ret;
    FILE* fp;
    //sprintf(path,"/fs/%s",LOG_FILE_NAME);


   // printf("read data in log file.. %s\n",path);

    fp = open_file(path);

    if(!fp){
        return false;
    }

    while (!feof(fp)) {
        int c = fgetc(fp);
        printf("%c", c);
    }

    close_file(fp);
    return true;
}

void SDcardProcess::display_directory()
{
    int ret;
    // Display the root directory
	printf("Opening the root directory... ");
	fflush(stdout);
	DIR* d = opendir("/fs/");
	printf("%s\n", (!d ? "Fail :(" : "OK"));
	if (!d) {
		error("error: %s (%d)\n", strerror(errno), -errno);
	}

	printf("root directory:\n");
	while (true) {
		struct dirent* e = readdir(d);
		if (!e) {
			break;
		}

		printf("    %s\n", e->d_name);
	}

	printf("Closing the root directory... ");
	fflush(stdout);
	ret = closedir(d);
	printf("%s\n", (ret < 0 ? "Fail :(" : "OK"));
	if (ret < 0) {
		printf("error: %s (%d)\n", strerror(errno), -errno);
	}
}

bool SDcardProcess::close_file(FILE *fp){
    int ret;
    // Close the file which also flushes any cached writes
    printf("Closing \"/fs/light_log.txt\"... ");
    
    if( (ret = fclose(fp))<0)
    {
        printf("Failed\nError %d during closing file: %s.\n",-errno,strerror(errno));
        return false;
    }

    printf("OK.\n");
}

FILE* SDcardProcess::open_file(char* path)
{
    static FILE* fp;

    printf("Opening file %s...",path);
    if(( fp = fopen(path,"r"))==NULL){
        printf("Failed. Error %d: %s\n",-errno,strerror(errno));
        return NULL;
    }

    printf("OK.\n");
    return fp;
}