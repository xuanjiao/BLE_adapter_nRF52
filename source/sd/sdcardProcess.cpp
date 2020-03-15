
#include "sdcardProcess.h"

// Print log info in Terminal 1. Log file located in "D:\Program Files (x86)\SEGGER\JLink"
uint8_t *data1Buffer[128];

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

//SEGGER_RTT_WriteString(j, buf);
SDcardProcess::SDcardProcess(events::EventQueue &event_queue)
{
    //SEGGER_RTT_ConfigUpBuffer(1, "DATA1", data1Buffer, 128, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
     _event_queue = &event_queue;

    _bd = BlockDevice::get_default_instance();

    // Set file system name
    _fs = new FATFileSystem(MBED_CONF_APP_FILE_SYSTEM_NAME);

    // Set button 1 as erase button.
    _irq = new InterruptIn(MBED_CONF_APP_PIN_ERASE_SD_CARD);
    
    _irq->fall(
        event_queue.event(this,&Self::remove_all_log_file)
    );

}

void SDcardProcess::add_log_file(Sensor_type type,int index)
{
    log_file file;
    file.type = type;
    file.index = index; 

    switch (type)
    {
        case light:
            sprintf(file.path,"/%s/light_list_%d.txt",MBED_CONF_APP_FILE_SYSTEM_NAME,index);
            break;
        case movement:
            sprintf(file.path,"/%s/accumulate_list_%d.txt",MBED_CONF_APP_FILE_SYSTEM_NAME,index);
            break;
        default:
            sprintf(file.path,"/%s/unknown_list_%d.txt",MBED_CONF_APP_FILE_SYSTEM_NAME,index);
            break;
    }  

    _log_files.push_back(file);
    
    printf("Add log file %s...",file.path); 
    
    FILE* fp = open_file(file.path,"a+");

    if(!fp){
        printf("Error %u during add log file %s: %s\n",-errno,strerror(errno));   
        return;
    }

    close_file(fp);

    set_current_log_file(file);

    printf("OK.\n");

    display_root_directory();
}


void SDcardProcess::print_sd_card_info()
{
    printf("sd size: %llu\n",         _bd->size());
    printf("sd read size: %llu\n",    _bd->get_read_size());
    printf("sd program size: %llu\n", _bd->get_program_size());
    printf("sd erase size: %llu\n",   _bd->get_erase_size());
}

bool SDcardProcess::remove_all_log_file(){

    display_root_directory();

    for (std::vector<log_file>::iterator it =  _log_files.begin() ; it !=  _log_files.end(); ++it)
    {
        printf("Removing log file %s\n",(*it).path);
        if( remove((*it).path)!= 0)
        {
            printf("Failed.\n Error %d\n: %s",-errno,strerror(errno));
            return false;
        }
        printf("OK.\n");
    }
    // printf("Removing log file %s\n",_my_log.path);

    display_root_directory();

    return true;
}
        
bool SDcardProcess::init_sd_card(){
    int ret;
    
	// Try to mount the filesystem
	printf("Mounting the filesystem... ");
	fflush(stdout);
	int err = _fs->mount(_bd);
	printf("%s\n", (err ? "Failed." : "OK"));
	if (err) {
		// Reformat if we can't mount the filesystem
		// this should only happen on the first boot
		printf("No filesystem found, formatting... ");
		fflush(stdout);
		err = _fs->reformat(_bd);
		printf("%s\n", (err ? "Fail." : "OK"));
		if (err) {
			printf("error: %s (%d)\n", strerror(-err), err);
		}
	}
    return true;
}

void SDcardProcess::set_current_log_file(log_file &file){
    _my_log = file;
}

void SDcardProcess::write_sensor_value_and_time(Sensor_type type,uint8_t value,char* time){
    int n;
    FILE *fp;
    char buffer[512];
    
    // Construct a line of data: value time
    n = sprintf(buffer,"%x %s\r\n",value,time);


    if( (fp = open_file(_my_log.path,"a+"))==NULL){
        return;
    }

    // Clear output buffer
    fflush(stdout);

    if( (n = fprintf(fp,buffer))<0)
    {
        printf("Error %d during write data to file: %s.\n",-errno,strerror(errno));
        return;
    }
    
   // printf("OK. wrote %d bytes\n",n);

    close_file(fp);
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
    if(strlen(_my_log.path)==0){
        printf("File path \'%s\' is invalid",_my_log.path);
        return false;
    }

    if( (fp = open_file(_my_log.path,"r")) == NULL){
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

    struct dirent e;

    // Read until reach the end of directory.
	while (d->read(&e)) {
        // display file name and file type
		printf("    %s   %x\n", e.d_name,e.d_type);
	}

	// printf("Closing the root directory... ");
	fflush(stdout);

	if ( closedir(d) < 0) {
		printf("Error %d: %s\n", -errno, strerror(errno));
        return false;
	}
    // printf("OK.\n");

    return true;
}

bool SDcardProcess::close_file(FILE *fp){

    // Close the file which also flushes any cached writes
   // printf("Closing \"/fs/light_log.txt\"... ");
    
    if( fclose(fp)<0)
    {
        printf("Error %d during closing file: %s.\n",-errno,strerror(errno));
        return false;
    }

  //  printf("OK.\n");
    return true;
}

FILE* SDcardProcess::open_file(const char *file_path,const char *mode)
{
    static FILE* fp;
    if(( fp = fopen(file_path,mode))==NULL){
        printf("Error %d during open file %s : %s\n",-errno,_my_log.path,strerror(errno));
        return NULL;
    }
    return fp;
}
