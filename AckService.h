
#define MAC_ADDR_SIZE_B (6)

// Template allows you to use 'constant' values on non-constant places (XD)
template<uint8_t dataSize>

class ACKService{
    public:
        const static uint16_t ACK_SERVICE_UUID = 0xA000;
        const static uint16_t ACK_CHARA_UUID = 0xA001;
        const static uint16_t ACK_MAC_CHAR_UUID = 0xA002;

        ACKService(BLEDevice &_ble, uint8_t *_initValues) : ble(_ble), ACK(ACK_CHARA_UUID, _initValues), MAC(ACK_MAC_CHAR_UUID, _initValues, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY){
                GattCharacteristic *charTable[] = {&ACK, &MAC};     // Add characteristick in table
                GattService AckService(ACK_SERVICE_UUID, charTable, sizeof(charTable)/sizeof(GattCharacteristic *));   
                ble.addService(AckService); // Add service in the BLE
        }
        void updateMacAddress(uint8_t  *MacAddress){
            ble.gattServer().write(MAC.getValueHandle(), MacAddress, MAC_ADDR_SIZE_B);
        }
    private:
        const static uint16_t ACK_CHARA_UUID = 0xA001;
        BLEDevice &ble;
        // Create new characteristic
        WriteOnlyArrayGattCharacteristic<uint8_t, dataSize> ACK;
        ReadOnlyArrayGattCharacteristic<uint8_t, MAC_ADDR_SIZE_B> MAC;
};