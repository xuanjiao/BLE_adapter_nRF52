#include "EnviromentSensingServer.h"


#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

int MAX_READ_BYTES = 100;

static void create_test_file(){
    FILE *fp1 = fopen("/fs/test_1.txt","w");
    if(!fp1){
        printf("failed to create test file\n");
        return;
    }

    for(int i = 0; i < 100;i++){
        fprintf(fp1,"%d %d\n",i,i*200);
    }
    fclose(fp1);

     FILE *fp2 = fopen("/fs/test_2.txt","w");
    if(!fp2){
        printf("failed to create test file\n");
        return;
    }

    for(int i = 0; i < 100;i++){
        fprintf(fp2,"%d %d\n",i,i*200);
    }
    fclose(fp2);
}

void EnviromentSensingServer::start(BLE &ble_interface,events::EventQueue& event_queue)
{
    _ble_interface = &ble_interface;
    _event_queue = &event_queue;
    _gattServer = &_ble_interface->gattServer();
    create_enviroment_sensing_service();
    create_file_transfer_characteristics();
    create_file_transfer_service();
    
}

void EnviromentSensingServer::create_enviroment_sensing_service()
{

    GattCharacteristic* bs_char_list[] = {
            new SensorCharacteristic(uuids[Sensor_type::light]),
            new SensorCharacteristic(uuids[Sensor_type::magnetometer])
    };

     // Register service.
    int num = sizeof(bs_char_list)/sizeof(GattCharacteristic*);
    printf("Adding Enviroment sensing service with %d characteristics\r\n",num);
    
    GattService service(GattService::UUID_ENVIRONMENTAL_SERVICE,(GattCharacteristic**)bs_char_list,num);

    ble_error_t error = _gattServer->addService(service);
    if(error){
        printf("Error %u during adding Enviroment sensing service.\r\n",error);
          return;
    } 
}

void EnviromentSensingServer::create_file_transfer_characteristics()
{
    
    DIR *d;
    char root_path[100]="";
    
    // Construct root directory eg./fs/
    sprintf(root_path,"/%s/",FILE_SYSTEM_NAME);

    // Open the root directory
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
		//printf("    %s\n", e.d_name);

        // if file name contain a sensor name
        for(int i = 0 ; i< SENSOR_TYPE_NUM ;i++){
            if(strstr(e.d_name,sensor_types[i] )&& i < MAX_CHARACTERISTIC_NUM){
                _num_of_file++;
                uint16_t uuid = file_char_uuid_base + (uint16_t)_num_of_file;
                
                // construct file path
                sprintf(file_buff[_num_of_file].path,"/%s/%s",FILE_SYSTEM_NAME,e.d_name);

                printf("log file found: [%s] generate uuid = 0x%x\n",file_buff[_num_of_file].path,uuid);
                
                uint8_t initial = (uint8_t)i;
                // generate file transfer characteristic
                file_chars_buff[_num_of_file-1] = new FileTransferCharacteristic(
                        uuid,
                        &initial);
            }
        }  
	}
    
	if ( closedir(d) < 0) {
		printf("Error %d: %s\n", -errno, strerror(errno));
        return;
	}
    printf("Closing root directory complete.\n");
    
}

void EnviromentSensingServer::create_file_transfer_service()
{
    create_test_file();

    // create a service
    GattService service(
            UUID::ShortUUIDBytes_t(file_service_uuid),
            (GattCharacteristic**)file_chars_buff,
            _num_of_file);

    ble_error_t error = _gattServer->addService(service);
    if(error){
        printf("Error %u during adding file transfer service.\r\n",error);
          return;
    } 

    // list all characteristics in file transfer service
    printf("characteristic list:\n");
    for(int i = 0; i < _num_of_file;i++){
        printf("[file no] %d [value attribute uuid] 0x%x [value handle] %d\n",
            i,
            file_chars_buff[i]->getValueAttribute().getUUID().getShortUUID(),
            file_chars_buff[i]->getValueHandle());
    }

   _gattServer->onDataRead(this, &Self::when_data_read_by_client);
}

void EnviromentSensingServer::when_receive_read_request(GattReadAuthCallbackParams* params)
{
    printf("receive ad read request\n");
}

void EnviromentSensingServer::when_data_read_by_client(const GattReadCallbackParams* params)
{
    printf("data read by client. Connect handle %d data handle %d\n",params->connHandle,params->handle);
    for(int i = 0; i < _num_of_file;i++){
        if(params->handle == file_chars_buff[i]->getValueHandle()){
            
            FILE *fp;
            // find respond file for this characteristic and open it
            fp = fopen(file_buff[i].path,"r");
            fseek(fp,file_buff[i].offset,SEEK_SET); // move offset bytes from beginning
            
            // read n bytes and store them in buffer.
            uint8_t buffer[MAX_READ_BYTES] = "";
            size_t n = fread(buffer,sizeof(uint8_t),MAX_READ_BYTES,fp);
            printf("read %d bytes from file %s:\n",n,file_buff[i].path);
            for(int i = 0; i< n;i++){
                printf("%d ",buffer[i]);
            }
            printf("\n");
            
            // write bytes in characteristic
            _gattServer->write(params->connHandle,params->handle,buffer,n);
        
            file_buff[i].offset += n;       
            fclose(fp);    
        }
    }
    
}

// void EnviromentSensingServer::updateFileCharacteristicValue(const ble::connection_handle_t connection_handle,
//                  file_index,const GattAttribute::Handle_t data_handle, const uint8_* value, uint16_t size)
// {
//     _gattServer->write(connection_handle,data_handle, value,size);
// }

// Update light value in ldr gatt characteristic.
void EnviromentSensingServer::update_sensor_value(Sensor_type type,uint8_t value,char* time){
    
    // switch (type)
    // {
    // case light :
    //     printf("got light = %x, time %s\r\n",value,time);
    //     _gattServer->write( 
    //         _light_char.getValueHandle(),
    //         &value,
    //         sizeof(light)
    //     );
    //     break;
    // case magnetometer :
    //     printf("got magnetometer = %x, time %s\r\n",value,time);
    // default:
    //     break;
    // }
    
}


template<typename Arg>
FunctionPointerWithContext<Arg> EnviromentSensingServer::as_cb(void (Self::*member)(Arg))
{
    return makeFunctionPointer(this, member);
}
