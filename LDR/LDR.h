#ifndef LDR_H
#define LDR_H

#include "mbed.h"
/**
 * Light sensor
 */
class LDR{
    public:
        LDR(AnalogIn pin);
        /** @returns
         *    value normalized to a byte [0,255]
         */
        uint8_t getLight();
        /** @returns
         *    value normalized to [0.0,1.0]
         */
        float getLight01();
        /** @returns
         *    value normalized to [0,100]
         */
        uint8_t getLightPercentage();
        /** @returns
         *    sensor voltage
         */
        float getLight_V();
        /** @returns
         *    raw sensor data
         */
        uint16_t getLightRaw();
    private:
        AnalogIn light;
};

#endif // LDR_H