/* 
 * aconno.de
 * Made by Jurica Resetar
 * All right reserved
 *
 */

#include "mbed.h"
#include "ble/BLE.h"
#include "acd52832_bsp.h"
#include "SEGGER_RTT.h"
#include "GapAdvertisingData.h"
#include "AckService.h"
#include "nrf52_uart.h"
#include "LDR/LDR.h"

#define SLEEP_TIME      (1.0)           /* Sleep time in seconds */
#define WAKE_UP_TIME    (100)          /* Awake time in ms */
#define MSD_SIZE        (19)             /* Manufacturer Specific Data lenght (in B) */
#define ADV_INTERVAL    (20)          /* Advertising interval in ms */
#define TX_POWER        (4)             /* TX power (in dB) */
#define DEVICE_NAME     "ACD"

#define GO_TO_SLEEP     (0)             /* Sleep flag: 0 -> Device will not go to sleep, 1 -> Will go to sleep mode */
#define PRINT           (0)

#define printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)

#if PRINT
    #define TX          (p25)
    #define RX          (p26)
    NRF52_UART serial(TX, RX, Baud9600);
    char buffer[sizeof(char)*255];
    uint8_t dataWritten;
#endif

bool SLEEP = true;

ACKService<4> *ackServicePtr;
LDR ldr(ADC_LIGHT);
static const uint16_t uuid16_list[] = {ACKService<4>::ACK_SERVICE_UUID};

DigitalOut advLED(p22);         // Red
DigitalOut scanLED(p23);        // Blue
DigitalOut connectedLED(p24);   // Green
    
GapAdvertisingData adv_data = GapAdvertisingData();
uint8_t MSD[MSD_SIZE] = {0x00, 0x59};
uint8_t myMacAddress[6] = {};

void onConnectionCallback(const Gap::ConnectionCallbackParams_t *params){   
        scanLED = !scanLED;       // Blue
        wait_ms(100);
        scanLED = !scanLED;       // Blue
        wait_ms(100);
        scanLED = !scanLED;       // Blue
        wait_ms(100);
        scanLED = !scanLED;       // Blue
        wait_ms(100);
        scanLED = !scanLED;       // Blue
        wait_ms(100);
        scanLED = !scanLED;       // Blue
        wait_ms(100);
        scanLED = 1;              // Blue
}

void onDataWrittenCallback(const GattWriteCallbackParams *params) {
    /*
        This function should make advertiser stop
    */
    //dataWritten = sprintf(buffer, "Bok bok =) \n\r");
    //serial.send(buffer, dataWritten);
    
    //ble.disconnect(Gap::LOCAL_HOST_TERMINATED_CONNECTION);
    return;
}

/**
 * Restart Advertising on disconnection
 */
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params){
    BLE::Instance().gap().startAdvertising();
}

/**
 *  Function for waking the core up
 */
void wakeMeUp(void){
        SLEEP = false;
    }

/**
 * This function is called when the ble initialization process has failed
 */
void onBleInitError(BLE &ble, ble_error_t error){
    /* Avoid compiler warnings */
    (void) ble;
    (void) error;
    /* Initialization error handling should go here */
}

/**
 * Callback triggered when the ble initialization process has finished
 */
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params){
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    
    
    uint8_t init_values[4] = {0,0,0,0};
    /* Get my MAC address */
    BLEProtocol::AddressType_t temp_address_type;
    ble.gap().getAddress(&temp_address_type, myMacAddress);
    ackServicePtr = new ACKService<4>(ble, init_values);
    ackServicePtr->updateMacAddress(myMacAddress);    // Update MAC address
    
    ble.gap().onDisconnection(disconnectionCallback);
    ble.gap().onConnection(onConnectionCallback);         // -->> Uncomment these two lines for shush-shush 
    ble.gattServer().onDataWritten(onDataWrittenCallback);
    
    /* setup advertising */
    //ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, (uint8_t *)MSD, MSD_SIZE);
    ble.gap().setAdvertisingInterval(500);  // --> Has to be at least 100ms!
    ble.gap().startAdvertising();

}

void updateData(BLE *ble){    
    static uint8_t simple_counter = 0;
    
    // Value normalized to a byte [0,255]
    uint8_t uLight = ldr.getLight();
    printf("light = %x\n",uLight);

    MSD[2] = simple_counter++;
    adv_data = ble->getAdvertisingData();
    adv_data.updateData(adv_data.MANUFACTURER_SPECIFIC_DATA, &uLight, sizeof(uint8_t));
    ble->setAdvertisingData(adv_data);
}


int main(void){
    
    BLE &ble = BLE::Instance();
    
    advLED = 1;         // Red
    scanLED = 1;        // Blue
    connectedLED = 1;   // Green

    Ticker ticker;
    ticker.attach(wakeMeUp, SLEEP_TIME);   // Wake the device up   
    
    ble.init(bleInitComplete);
    ble.gap().setTxPower(TX_POWER);        // Set TX power to TX_POWER
    
    /* SpinWait for initialization to complete. This is necessary because the
     * BLE object is used in the main loop below. */
    while (ble.hasInitialized()  == false) { /* spin loop */ }
    while (true){
        if (SLEEP && GO_TO_SLEEP){
            ble.gap().stopAdvertising();
            sleep();
            ble.waitForEvent();
        }
        else{
            // I'm awake
            updateData(&ble);
            ble.gap().startAdvertising();
            wait_ms(WAKE_UP_TIME);
            ble.waitForEvent();
            SLEEP = true;
            }
    }
}
