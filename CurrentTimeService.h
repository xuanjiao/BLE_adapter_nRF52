#include "ble/BLE.h"
#include <GattCharacteristic.h>
#include <GattService.h>
typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
}BLE_DateTime;

#define BLE_CURRENT_TIME_CHAR_VALUE_SIZE 10

class CurrentTimeService
{
    private:
    /* data */
    
    /*GattCharacteristic	(	const UUID & 	uuid,
            uint8_t * 	valuePtr = NULL,
        uint16_t 	len = 0,
        uint16_t 	maxLen = 0,
        uint8_t 	props = BLE_GATT_CHAR_PROPERTIES_NONE,
        GattAttribute * 	descriptors[] = NULL,
        unsigned 	numDescriptors = 0,
        bool 	hasVariableLen = true 
)	
    */
    public:
    CurrentTimeService(BLE &_ble,BLE_DateTime &initialDateTime):
        ble(_ble),
        currentTimeCharacteristic(
                                    /* UUID */ GattCharacteristic::UUID_CURRENT_TIME_CHAR,
                                    /* Initial value */valueBytes,
                                    /* Value size */ BLE_CURRENT_TIME_CHAR_VALUE_SIZE,
                                     /* Value capacity */BLE_CURRENT_TIME_CHAR_VALUE_SIZE,
                                    /* Properties */GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
                                                 | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
                                                 | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE,
                                    /* Descriptors */ NULL,
                                    /* Num descriptors */ 0,
                                    /* variable len */ false
                                )                   
        {
            // Write datetime value to buffer array    
            writeDateTimeToBuffer(initialDateTime);
            //ticker.attach()

            GattCharacteristic *characteristics[] = {&currentTimeCharacteristic};
            GattService currentTimeService(GattService::UUID_CURRENT_TIME_SERVICE,
                                            characteristics,
                                            sizeof(characteristics));
            ble.addService(currentTimeService);
        }

    
    void writeDateTimeToBuffer(BLE_DateTime &dateTime)
    {
        *(uint16_t*)&valueBytes[0] = dateTime.year;
        valueBytes[2]=dateTime.month;
        valueBytes[3]=dateTime.day;
        valueBytes[4]=dateTime.hours;
        valueBytes[5]=dateTime.minutes;
        valueBytes[6]=dateTime.seconds;

        // reserve
        valueBytes[7] = 0x00;   
        valueBytes[8] = 0x00;   
        valueBytes[9] = 0x00;  
    }


    protected:
        Ticker ticker;
        BLE &ble;
        uint8_t valueBytes[BLE_CURRENT_TIME_CHAR_VALUE_SIZE];
        GattCharacteristic currentTimeCharacteristic;
};