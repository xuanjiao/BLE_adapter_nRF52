
#include "sdcardProcess.h"

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

SDcardProcess::SDcardProcess()
{
    _bd = BlockDevice::get_default_instance();
    _fs = new FATFileSystem(MBED_CONF_APP_FILE_SYSTEM_NAME);
    //_irq = new InterruptIn(MBED_CONF_APP_PIN_ERASE_SD_CARD);
    sprintf(file_path,"/%s/%s",MBED_CONF_APP_FILE_SYSTEM_NAME,MBED_CONF_APP_LOG_FILE_NAME);
}

void SDcardProcess::print_sd_card_info(){
    printf("sd size: %llu\n",         _bd->size());
    printf("sd read size: %llu\n",    _bd->get_read_size());
    printf("sd program size: %llu\n", _bd->get_program_size());
    printf("sd erase size: %llu\n",   _bd->get_erase_size());
}

bool SDcardProcess::erase_sd_card(){
    printf("Initializing the sd card...");

    if(_bd->init()!=0)
    {
        printf("Failed.\nError %d during initializing sd card: %s",-errno,strerror(errno));
    }
    printf("OK.\nErasing SD card...");

    if( _bd->erase(0,_bd->get_erase_size()) != 0)
    {
        printf("Failed.\nError %d during erasing block\n: %s",-errno,strerror(errno));
        return false;
    }
    printf("OK.\nDeinitializing the block device...");

    if( _bd->erase(0,_bd->deinit()) != 0)
    {
        printf("Failed.\nError %d during deinitializing block device\n: %s",-errno,strerror(errno));
        return false;
    }
    printf("OK.\n");

    return true;
}
        
bool SDcardProcess::init_sd_card(){
    int ret;
    
	// Try to mount the filesystem
	printf("Mounting the filesystem... ");
	fflush(stdout);
	int err = _fs->mount(_bd);
	printf("%s\n", (err ? "Fail :(" : "OK"));
	if (err) {
		// Reformat if we can't mount the filesystem
		// this should only happen on the first boot
		printf("No filesystem found, formatting... ");
		fflush(stdout);
		err = _fs->reformat(_bd);
		printf("%s\n", (err ? "Fail :(" : "OK"));
		if (err) {
			error("error: %s (%d)\n", strerror(-err), err);
		}
	}

    /*
    printf("Mounting the filesystem...");
    fflush(stdout);
    if((ret = (_fs->mount(_bd)))!=0){
        printf("Failed.\nError %d during mounting the filesystem: %s, ret = %d\n",-errno,strerror(errno),ret);
        printf("No system found, formating...");
        fflush(stdout);

        // Reformating system
        if( _fs->reformat(_bd)!=0){
            printf("Failed.\nError %d during reformat the sd card: %s.\n",-errno,strerror(errno));
            return false;
         }
        printf("OK.\n");
    }
    
    printf("OK.\n");
    */
    return true;

}

bool SDcardProcess::write_sensor_value_and_time(int value,char* time){
    int n;
    FILE *fp;
    char buffer[512];
    
    // Construct a line of data: value time
    n = sprintf(buffer,"%d %s",value,time);

    if( (fp = open_file(file_path,"w+"))==NULL){
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

    close_file(fp);
    return true;
}

bool SDcardProcess::close_sd_card(){
    
    printf("Unmounting... ");

    if(_fs->unmount()<0)
    {
        printf("Failed\nError %d during unmount: %s.\n",-errno,strerror(errno));
        return false;
    }
    
    printf("OK.\n");

    return true;
}

bool SDcardProcess::read_log_file(){
    FILE* fp;

    // Check whether file path is empty
    if(strlen(file_path)==0){
        printf("File path \'%s\' is invalid",file_path);
        return false;
    }

    if( (fp = open_file(file_path,"r")) == NULL){
        return false;
    }

    // Get char until reach the end-of-file indicator.
    while (!feof(fp)) {

        // Get the char point by internal file pointer, pointer then point to next char.
        int c = fgetc(fp);
        printf("%c", c);
    }

    close_file(fp);
    return true;
}

bool SDcardProcess::display_root_directory()
{
    DIR *d;
    char root_path[PATH_LEN_MAX]="";
    
    // Construct root directory eg./fs/
    sprintf(root_path,"/%s/",MBED_CONF_APP_FILE_SYSTEM_NAME);

    // Display the root directory
	printf("Opening the root directory... ");
	fflush(stdout);

	if ((d = opendir(root_path))==NULL) {
		printf("Failed.\nError %d during open directory: %s\n",  -errno,strerror(errno));
        return false;
	}else{printf("OK.\n");}

	printf("root directory:\n");

    struct dirent e;

    // Read until reach the end of directory.
	while (d->read(&e)) {
        // display file name and file type
		printf("    %s   %s\n", e.d_name,e.d_type);
	}

	printf("Closing the root directory... ");
	fflush(stdout);

	if ( closedir(d) < 0) {
		printf("Failed.\nError %d: %s\n", -errno, strerror(errno));
        return false;
	}
    printf("OK.\n");
    return true;
}

bool SDcardProcess::close_file(FILE *fp){

    // Close the file which also flushes any cached writes
    printf("Closing \"/fs/light_log.txt\"... ");
    
    if( fclose(fp)<0)
    {
        printf("Failed\nError %d during closing file: %s.\n",-errno,strerror(errno));
        return false;
    }

    printf("OK.\n");
    return true;
}

FILE* SDcardProcess::open_file(const char *file_path,const char *mode)
{
    static FILE* fp;

    printf("Opening file \"%s\"...",file_path);
    if(( fp = fopen(file_path,mode))==NULL){
        printf("Failed.\nError %d: %s\n",-errno,strerror(errno));
        return NULL;
    }

    printf("OK.\n");
    return fp;
}
