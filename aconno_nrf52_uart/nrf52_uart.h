/** Made by: Karlo Milicevic, 2017. 
 * aconno.de
 */

#ifndef NRF52_UART_H
#define NRF52_UART_H

#include "mbed.h"

enum Baudrate{
    Baud1200   = 0x0004F000,
    Baud2400   = 0x0009D000,
    Baud4800   = 0x0013B000,
    Baud9600   = 0x00275000,
    Baud14400  = 0x003AF000,
    Baud19200  = 0x004EA000,
    Baud28800  = 0x0075C000,
    Baud38400  = 0x009D0000,
    Baud57600  = 0x00EB0000,
    Baud76800  = 0x013A9000,
    Baud115200 = 0x01D60000,
    Baud230400 = 0x03B00000,
    Baud250000 = 0x04000000,
    Baud460800 = 0x07400000,
    Baud921600 = 0x0F000000,
    Baud1M     = 0x10000000,
};

class NRF52_UART{
    public:
        NRF52_UART(PinName TX, PinName RX, Baudrate baudrate);
        bool send(uint8_t *buffer, uint8_t length);
        inline bool send(char *buffer, uint8_t length){
            return send((uint8_t *)buffer, length);
        }
        /** Unimplemented
         */
        bool receive(uint8_t *buffer, uint8_t length);
    private:
};

#endif // NRF52_UART_H