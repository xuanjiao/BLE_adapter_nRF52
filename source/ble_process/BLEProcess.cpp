
    
    #include "BLEProcess.h"

    #define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)
    
    BLEProcess::~BLEProcess()
    {
        stop();
    }

        // Subscripton_inition to the ble interface initialization event.
    void BLEProcess::on_init(mbed::Callback<void(BLE&,events::EventQueue&)> cb)
    {        
        _post_init_cbs[_count_init_cb] = cb;
        _count_init_cb++;
    }

    void BLEProcess::on_connection_complete(mbed::Callback<void()> cb)
    {
        _post_connection_compete_cb = cb;
    }

    
    // Initialize the ble interface, configure it and start advertising.
    bool BLEProcess::start()
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
            makeFunctionPointer(this,&BLEProcess::schedule_ble_events)
        );
        

       printf("init..\n");
        ble_error_t error = _ble_interface.init(this,&BLEProcess::when_init_complete);
        if(error){
            printf("Error: %u returned by BLE::init.\r\n", error);
            return false;
        }
      
        return true;
    }

    // Close existing connection and stop process
    void BLEProcess::stop()
    {
        if(_ble_interface.hasInitialized()){
            _ble_interface.shutdown();
            printf("BLE process stopped");
        }
    }

    // Sets up adverting payload and start advertising.
    void BLEProcess::when_init_complete(BLE::InitializationCompleteCallbackContext *event)
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

        advertise();


/*
        gap.setAdvertisingParams(make_advertising_params());
        gap.onConnection(this, &BLEProcess::when_connection);
        gap.onDisconnection(this, &BLEProcess::when_disconnection);

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

    void BLEProcess::advertise()
    {
        ble_error_t error;

        // set advertising parameters, which defined in head file.
        error = _gap.setAdvertisingParameters(
            ble::LEGACY_ADVERTISING_HANDLE,
            ble::AdvertisingParameters(
                advertising_params.type,
                advertising_params.min_interval, 
                advertising_params.max_interval 
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

    }


    // Start the gatt client process when a connection event is received.
     void BLEProcess::when_connection(const Gap::ConnectionCallbackParams_t *connection_event)
     {
         printf("Connected. \r\n");
       
         if(_post_connection_compete_cb){
             _post_connection_compete_cb();
         }
         
     }

    // Stop the gatt client process when the device is disconnected then restart
    // advertising.
     void BLEProcess::when_disconnection(const Gap::DisconnectionCallbackParams_t *event)
     {
         printf("Disconnected.\r\n");
         start_advertising();
     }

    // Schedule processing of events from the BLE middleware in the event queue.
    void BLEProcess::schedule_ble_events(BLE::OnEventsToProcessCallbackContext *event)
    {
        _event_queue.call(mbed::callback(&event->ble, &BLE::processEvents));
    }
