/*
 * 
 */

#include "LDR.h"
#include "utilities.h"

#define ADC_RESOLUTION_BITS 10 /* NOTE: May be 10 or 11 */
#define ADC_VOLTAGE_RESOLUTION_V REF_VOLTAGE_V/(1<<ADC_RESOLUTION_BITS)

LDR::LDR(AnalogIn pin) : light(pin){}

uint8_t LDR::getLight(){
    return (uint8_t)(light.read() * 255);
}

float LDR::getLight01(){
    return light.read();
}

uint8_t LDR::getLightPercentage(){
    return (uint8_t)(light.read() * 100);
}

float LDR::getLight_V(){
    return light.read_u16() * ADC_VOLTAGE_RESOLUTION_V;
}

uint16_t LDR::getLightRaw(){
    return light.read_u16();
}
