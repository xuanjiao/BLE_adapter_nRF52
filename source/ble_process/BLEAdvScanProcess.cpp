
    
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

    void BLEAdvScanProcess::on_connection_complete(mbed::Callback<void(device_t)> cb)
    {
        _post_connection_compete_cb = cb;
    }

    
    // Initialize the ble interface, configure it and start advertising.
    bool BLEAdvScanProcess::start()
    {
        printf("Ble advertise and scan process start\n");
        
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
        
        mode_start();

        if (_count_init_cb) {
            for(int i = 0;i <_count_init_cb;i++){
            
                // Here run init func from gatt client and server.
                _post_init_cbs[i](_ble_interface,_event_queue);

            }
        }
        
    }

    void BLEAdvScanProcess::mode_start()
    {
        if(!_is_in_scanning_mode){
            // if adv signal is received, start advertising   
            _led_adv = 0; // led adv on
            _event_queue.call(this,&BLEAdvScanProcess::advertise);
        } else {
             // if no adv signal start scan
             _led_scan = 0; // led scan on
            _event_queue.call(this,&BLEAdvScanProcess::scan);
        }

        // connection state
        _is_connecting = false;

        // queue up next mode
        _on_duration_end_id = _event_queue.call_in(
            MODE_DURATION_MS,
            this,
            &Self::mode_end         
        );

    }

    void BLEAdvScanProcess::mode_end()
    {
        ble_error_t error;

        // stop mode change
        _event_queue.cancel(_on_duration_end_id);

        if(_is_in_scanning_mode){
            _led_scan = 1; //led scan off

            // if it is scanning, stop scan
            _gap.stopScan();
        }else{
            _led_adv = 1; //led adv off
            
            // if it is advertising, stop advertise
            _gap.stopAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
        }

        // Change to another mode
        _is_in_scanning_mode = !_is_in_scanning_mode;

        _event_queue.call_in(
            TIME_BETWEEN_MODES_MS,
            this,
            &BLEAdvScanProcess::mode_start);
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
        printf("stop advertising.\n");
        _led_adv = 1; //led adv off
    }

    void BLEAdvScanProcess::scan(){
        printf("set up scan\n");
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

        // Start scan
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
        bool found = false;
        // look at events from deviced at a close range
        if(event.getRssi()<-75){
            return;
        }

        ble::AdvertisingDataParser adv_parser(event.getPayload());

        //parse the advertising payload
        while(adv_parser.hasNext()){
            ble::AdvertisingDataParser::element_t field = adv_parser.next();
               
            if(field.type == ble::adv_data_type_t::COMPLETE_LOCAL_NAME){

                // Transfer device name to string and print it
                char value_str[100];
                strncpy(value_str,(char*)field.value.data(),field.value.size());
                printf("[Name] %s [address] ",value_str);
                print_address(event.getPeerAddress().data());
                
            }else if(field.type == ble::adv_data_type_t::INCOMPLETE_LIST_16BIT_SERVICE_IDS){
                // Print incomplete 16 bit service uuid
                uint16_t uuid = *(uint16_t*)field.value.data();
                printf("[incomplete list 16 bit service id: 0x%.2x\n",uuid);

                // found target beacon
                if(uuid == CC2650_ADV_UUID){
                    found = true;
                    break;
                }

            }else if(field.type == ble::adv_data_type_t::SERVICE_DATA){
                
                // print service data size
                printf("[Service data] size %d ",field.value.size());

                // print 16-bit service uuid
                uint16_t uuid = *(uint16_t*)field.value.data();
                printf("UUID: 0x%.2x data: ",uuid);
                
                // print service data value
                for(int i = 2; i < field.value.size(); i++){
                    printf("0x%.2x ",field.value[i]);
                }
                printf("\n");

                // Fund a device with CTS
                if(uuid == GattService::UUID_CURRENT_TIME_SERVICE){
                    
                    // set found flag, stop reading advertisement payload.
                    found = true;
                    break;
                }

            }else if(field.type ==  ble::adv_data_type_t::SERVICE_DATA_16BIT_ID ||
                      field.type ==   ble::adv_data_type_t::SERVICE_DATA_128BIT_ID
                ){
                    printf("[Service data uuid] %x\n",*(uint16_t*)field.value.data()); 
            }

        }

        // if target device is found, connect to it.
        if(found){                   
                    // // Stop scan and stop advertising
                    // _event_queue.cancel(_on_duration_end_id);

                    // connect led blink
                    _on_led_blinking_id = _event_queue.call_every(LED_BLINK_INTERVAL_MS,this,&Self::led_connect_change_state);

                    // Connect to peer
                    _event_queue.call<BLEAdvScanProcess,void,ble::peer_address_type_t,
                                            const ble::address_t&,
                                            const ble::ConnectionParameters&>(
                        this,
                        &Self::connectToPeer,
                        event.getPeerAddressType(),
                        event.getPeerAddress(),
                        ble::ConnectionParameters() // use the default connection parameters
                    );
        }
    }
	
    void BLEAdvScanProcess::connectToPeer(ble::peer_address_type_t peerAddressType,
                                            const ble::address_t & peerAddress,
                                            const ble::ConnectionParameters & connectionParams){
                            
        ble_error_t error = _gap.connect(peerAddressType, peerAddress,connectionParams);
                
        if (error){
            printf("Error %d caused by Gap::connect\n",error);
            // since no connection will be attempted end the mode
            _event_queue.call(this, &Self::mode_end);
            return;
        }

        // Connect to this device
        printf("Connected to ");
        print_address(peerAddress.data());
    }


    //  Called by Gap to notify the scan times out.
    void BLEAdvScanProcess::onScanTimeout(const ble::ScanTimeoutEvent &event)
    {
         printf("Stopped scanning\n");
         _led_scan = 1; // led scan stop
    }

    //Called when connection attempt ends or an advertising device has been connected.
    // void BLEAdvScanProcess::onConnectionComplete(const ble::ConnectionCompleteEvent &event)
    void BLEAdvScanProcess::onConnectionComplete(const ble::ConnectionCompleteEvent &event)
    {
        if(event.getStatus() != BLE_ERROR_NONE){
            printf("Connection error. status %d\n",event.getStatus());
            return;
        }
        
        printf("Connected to ");
        print_address(event.getPeerAddress().data());
        
        device_t new_device = {};
        new_device.connection_handle = event.getConnectionHandle();
        memcpy(new_device.address,event.getPeerAddress().data(),sizeof(new_device.address));
        
        
        //Continue advertise and find other periphrals 
        _event_queue.call(this, &Self::mode_end);

        _is_connecting = true;
        // Start gatt client service discovery
        if(_post_connection_compete_cb){
             _post_connection_compete_cb(new_device);
        }


    }

    // Called when a connection has been disconnected.
    void BLEAdvScanProcess::onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event)
    {
        // print handle
        printf("Disconnected. Connection handle %d. ",event.getConnectionHandle());

        // print reason
        if(event.getReason().value() == Gap::DisconnectionReason_t::LOCAL_HOST_TERMINATED_CONNECTION){
            printf("From base staion.\n");
        }else{
            printf("reason : %x\n",event.getReason().value());
        }
        
        // led connect stop blinkiing
        _event_queue.cancel(_on_led_blinking_id);
        
        //we have successfully disconnected. ending the demo, move to next mode 
        _event_queue.call(this, &Self::mode_end);
    }

    // Schedule processing of events from the BLE middleware in the event queue.
    void BLEAdvScanProcess::BLEAdvScanProcess::schedule_ble_events(BLE::OnEventsToProcessCallbackContext *event)
    {
        _event_queue.call(mbed::callback(&event->ble, &BLE::processEvents));
    }

    void BLEAdvScanProcess::print_address(const uint8_t *addr)
    {
        printf("%02x:%02x:%02x:%02x:%02x:%02x\r\n",
           addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    }

    void BLEAdvScanProcess::led_connect_change_state(){
        _led_connect = !_led_connect;
    }
