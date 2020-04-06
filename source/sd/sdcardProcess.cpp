
#include "sdcardProcess.h"

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

void SDcardProcess::record_beacon(Device_t &dev){
    // Check status
    if(!_ready_to_write){
        return;
    }

    for( int i = 0; i <dev.num_of_chars;i++){
        
        //record each chracteristic in a file
        Sensor_char_t* sensor_char = &dev.chars.sensor_chars[i];
        char file_path[50];

        time_t current_time = time(NULL);
         
        char date[15];
        char complete_time[50];
         // "H"hour(24h)|"M"minute(00-59)|"S"second(00-60)|"d"day(01-31)| "m"month |"Y"years| "u"day of week(1-7) | 
        strftime(complete_time,sizeof(complete_time),"%H:%M:%S %d-%m-%Y day of week %u",localtime(&current_time));
        strftime(date,sizeof(date),"%d-%m-%Y",localtime(&current_time));

        // consruct file name "/<file system name>/<sensor type>_<first byte of address>"
        sprintf(file_path,"/%s/%s_%.2d.txt",
                    FILE_SYSTEM_NAME,   
                    sensor_types[sensor_char->type],// sensor type           
                    dev.address[0]);  // first byte address
        
        
        printf("log data in file[%s]:",file_path);
        for(int i = 0; i < sensor_char->len;i++){
            printf("%d ",sensor_char->data[i]);
        }
        printf("\n");
        // open file
        FILE* fp;
        if(( fp = fopen(file_path,"a+"))==NULL){
            printf("Error %d during open file %s : %s\n",-errno,file_path,strerror(errno));
            return;
        }
        // write data in file
        for(int i = 0; i < sensor_char->len;i++){
            if(fprintf(fp,"%d ",sensor_char->data[i])<0)
                printf("Error %d during write data to file %s: %s.\n",-errno,file_path,strerror(errno));
        }
        fputc('\n',fp);
       
        // close file
        if( fclose(fp)<0){
            printf("Error %d during closing file: %s.\n",-errno,strerror(errno));
            return;
        }
        printf("OK.\n");           
    }     
}

void SDcardProcess::print_sd_card_info()
{
    printf("sd size: %llu\n",         _block_device->size());
    printf("sd read size: %llu\n",    _block_device->get_read_size());
    printf("sd program size: %llu\n", _block_device->get_program_size());
    printf("sd erase size: %llu\n",   _block_device->get_erase_size());
}
        
void SDcardProcess::init_sd_card(){

	// Try to mount the filesystem
	printf("Mounting the filesystem... ");
	fflush(stdout);

    // Get block device based on the configuration JSON. 
    _block_device = BlockDevice::get_default_instance();

	int err = _file_system.mount(_block_device);
	printf("%s\n", (err ? "Failed." : "OK"));
	if (err) {
		// Reformat if we can't mount the filesystem
		// this should only happen on the first boot
		printf("No filesystem found, formatting... ");
		fflush(stdout);
		err = _file_system.reformat(_block_device);
		printf("%s\n", (err ? "Fail." : "OK"));
		if (err) {
			printf("error: %s (%d)\n", strerror(-err), err);
            _ready_to_write = false;
            return;
		}
	}

    printf("Set up erase button..\n");    

    _ready_to_write = true;
    display_root_directory();
}

void SDcardProcess::reformat_sd_card(){
        _ready_to_write = false;
    	printf("Reformatting file system... ");
		
		int err = _file_system.reformat(_block_device);
		printf("%s\n", (err ? "Fail." : "OK"));
		if (err) {
			printf("error: %s (%d)\n", strerror(-err), err);
            return;
		}
        _ready_to_write = true;
        display_root_directory();
}

void SDcardProcess::unmound_sd_card(){
    
    printf("Unmounting... ");

    if(_file_system.unmount()<0)
    {
        printf("Failed\nError %d during unmount: %s.\n",-errno,strerror(errno));
        return;
    }
    
    printf("OK.\n");
    _ready_to_write = false;
}


void SDcardProcess::display_root_directory()
{
    DIR *d;
    char root_path[100]="";
    
    // Construct root directory eg./fs/
    sprintf(root_path,"/%s/",FILE_SYSTEM_NAME);

    // Display the root directory
	printf("Opening the root directory... ");
	fflush(stdout);

	if ((d = opendir(root_path))==NULL) {
		printf("Failed.\nError %d during open directory: %s\n",  -errno,strerror(errno));
        return;
	}else{printf("OK.\n");}

    struct dirent e;

    // Read until reach the end of directory.
	while (d->read(&e)) {
        // display file name and file type
		printf("    %s\n", e.d_name);
	}

	fflush(stdout);

	if ( closedir(d) < 0) {
		printf("Error %d: %s\n", -errno, strerror(errno));
        return;
	}
    
    printf("Closing root directory complete.\n");
}
