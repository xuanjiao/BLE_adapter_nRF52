
    
    #include "BLEAdvScanProcess.h"

    #define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)
    
    BLEAdvScanProcess::~BLEAdvScanProcess()
    {
        stop();
    }

        // Subscripton_inition to the ble interface initialization event.
    void BLEAdvScanProcess::on_init(mbed::Callback<void(BLE&,events::EventQueue&)> cb)
    {        
        _post_init_cbs[_count_init_cb] = cb;
        _count_init_cb++;
    }

    void BLEAdvScanProcess::on_connection_complete(mbed::Callback<void()> cb)
    {
        _post_connection_compete_cb = cb;
    }

    
    // Initialize the ble interface, configure it and start advertising.
    bool BLEAdvScanProcess::start()
    {
        printf("Ble process start\n");

        if(_ble_interface.hasInitialized()){
            printf("Error: the ble instance has already been initialized.\n");
            return false;
        }

        /* hande gap events*/
        _gap.setEventHandler(this);

        // It will be called every time the BLE stack has pending work.      
        _ble_interface.onEventsToProcess(
            makeFunctionPointer(this,&BLEAdvScanProcess::schedule_ble_events)
        );
        

       printf("init..\n");
        ble_error_t error = _ble_interface.init(this,&BLEAdvScanProcess::when_init_complete);
        if(error){
            printf("Error: %u returned by BLE::init.\r\n", error);
            return false;
        }
      
        return true;
    }

    // Close existing connection and stop process
    void BLEAdvScanProcess::stop()
    {
        if(_ble_interface.hasInitialized()){
            _ble_interface.shutdown();
            printf("BLE process stopped");
        }
    }

    // Sets up adverting payload and start advertising.
   
    void BLEAdvScanProcess::when_init_complete(BLE::InitializationCompleteCallbackContext *event)
    {
        if(event->error){
            printf("Error %u during the initialization\r\n", event->error);
            return;
        }
        printf("BLE instance initialized.\r\n");

        /* setup the default phy used in connection to bit rate of 2 Mb/s to reduce power consumption */
        if (_gap.isFeatureSupported(ble::controller_supported_features_t::LE_2M_PHY)) {
            ble::phy_set_t phys(/* 1M */ false, /* 2M */ true, /* coded */ false);

            ble_error_t error = _gap.setPreferredPhys(/* tx */&phys, /* rx */&phys);
            if (error) {
                printf("Error %x during GAP::setPreferedPhys\n",error);
            }
        }

       // advertise();
        scan();

/*
        gap.setAdvertisingParams(make_advertising_params());
        gap.onConnection(this, &BLEAdvScanProcess::when_connection);
        gap.onDisconnection(this, &BLEAdvScanProcess::when_disconnection);

        hier set scan parameters
        
        start_advertising();


        if (_count_init_cb) {
            for(int i = 0;i <_count_init_cb;i++){
            
                // Here run init func from gatt client and server.
                _post_init_cbs[i](_ble_interface,_event_queue);

            }
        }
        */
    }

    void BLEAdvScanProcess::advertise()
    {
        ble_error_t error;

        // set advertising parameters, which defined in head file.
        error = _gap.setAdvertisingParameters(
            ble::LEGACY_ADVERTISING_HANDLE,
            ble::AdvertisingParameters(
                adv_params.type,
                adv_params.min_interval, 
                adv_params.max_interval 
            )
        );

        if(error){
            printf("Error %d during Gap::setAdvertisingParameters()\n",error);
        }

        // max playload size get from _gap.getMaxAdvertisingDataLength()

        uint8_t adv_buffer[MAX_ADVERTISING_PAYLOAD_SIZE];
        
        ble::AdvertisingDataBuilder adv_data_builder(adv_buffer);

        // Add device name into the payload.
        adv_data_builder.setName(MBED_CONF_APP_BLE_DEVICE_NAME);

        uint8_t value[] = {0};
        // Add service uuid and data into the payload
        adv_data_builder.setServiceData(
            UUID(GattService::UUID_ENVIRONMENTAL_SERVICE),
            value
        );

        // Set advertising payload
        error = _gap.setAdvertisingPayload(
            ble::LEGACY_ADVERTISING_HANDLE,
            adv_data_builder.getAdvertisingData()
        );
        if(error){
            printf("Error %d during Gap::setAdvertisingPayload().\n",error);
        }

        // Start advertising.
        error = _gap.startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
        if (error) {
             printf("Error %d during Gap::startAdvertising()",error);
            return;
        }
        
        printf("Advertising started (type: 0x%x, interval: [%d : %d]ms)\r\n",
        adv_params.type.value(),
        adv_params.min_interval.valueInMs(), adv_params.max_interval.valueInMs() );
    }

    
    // Called when advertising ends.
    void BLEAdvScanProcess::onAdvertisingEnd(const ble::AdvertisingEndEvent &event)
    {
        if (event.isConnected()) {
            printf("Stopped advertising early due to connection\r\n");
        }
    }

    void BLEAdvScanProcess::scan(){
        ble_error_t error;

        error = _gap.setScanParameters(
            ble::ScanParameters(
                ble::phy_t::LE_1M,
                scan_params.interval,
                scan_params.window,
                scan_params.active
            )
        );

        if(error){
            printf("Error %d during Gap::setScanParameters.\n",error);
            return;
        }

        error = _gap.startScan(scan_params.duration);
        if(error){
            printf("Error %d during Gap::startScan\n",error);
            return;
        }

        printf("Scanning started (interval: %dms, window: %dms, timeout: %dms).\r\n",
            scan_params.interval.valueInMs(), scan_params.window.valueInMs(), scan_params.duration.valueInMs());
    }


    // Called when a scanner receives an advertising or a scan response packet.
    void BLEAdvScanProcess::onAdvertisingReport(const ble::AdvertisingReportEvent &event)
    {
        // cound scan event 
      //  _scan_count++;
       // printf("Scan event %d\n",_scan_count);

        // look at events from deviced at a close range
        if(event.getRssi()<-65){
            return;
        }



        ble::AdvertisingDataParser adv_parser(event.getPayload());

        //parse the advertising payload, looking for a discoverable device
        while(adv_parser.hasNext()){
            ble::AdvertisingDataParser::element_t field = adv_parser.next();
           
            char value_str[100];
            if(field.type == ble::adv_data_type_t::COMPLETE_LOCAL_NAME){
                strncpy(value_str,(char*)field.value.data(),field.value.size());
                printf("[Name] %s [address] ",value_str);
                print_address(event.getPeerAddress().data());
                
            }else if(field.type == ble::adv_data_type_t::SERVICE_DATA){
                
                // print service data size
                printf("[Service data] size %d ",field.value.size());

                // print 16-bit service uuid
                printf("UUID: 0x%.2x data: ",*(uint16_t*)field.value.data());
                
                // print service data value
                for(int i = 2; i < field.value.size(); i++){
                    printf("0x%.2x ",field.value[i]);
                }
                printf("\n");
            }else if(field.type ==  ble::adv_data_type_t::SERVICE_DATA_16BIT_ID ||
                      field.type ==   ble::adv_data_type_t::SERVICE_DATA_128BIT_ID
                ){
                    printf("[Service data uuid] %x\n",*(uint16_t*)field.value.data()); 
            }


        }
    }

    // Called when scan times out.
    void BLEAdvScanProcess::onScanTimeout(const ble::ScanTimeoutEvent &event)
    {
         printf("Stopped scanning early due to timeout parameter\r\n");
    }

    // Start the gatt client process when a connection event is received.
     void BLEAdvScanProcess::when_connection(const Gap::ConnectionCallbackParams_t *connection_event)
     {
         printf("Connected. \r\n");
       
         if(_post_connection_compete_cb){
             _post_connection_compete_cb();
         }
         
     }

    // Stop the gatt client process when the device is disconnected then restart
    // advertising.
     void BLEAdvScanProcess::when_disconnection(const Gap::DisconnectionCallbackParams_t *event)
     {
         printf("Disconnected.\r\n");
         start_advertising();
     }

    // Schedule processing of events from the BLE middleware in the event queue.
    void BLEAdvScanProcess::BLEAdvScanProcess::schedule_ble_events(BLE::OnEventsToProcessCallbackContext *event)
    {
        _event_queue.call(mbed::callback(&event->ble, &BLE::processEvents));
    }

    void BLEAdvScanProcess::print_address(const uint8_t *addr)
    {
     SEGGER_RTT_printf(0,"%02x:%02x:%02x:%02x:%02x:%02x\r\n",
           addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    }
