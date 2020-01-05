/* 
 * aconno.de
 * Made by Karlo Milicevic @ aconno, 2017
 * All rights reserved 
 *
 */

#ifndef NRF52_DIGITAL_H
#define NRF52_DIGITAL_H

#include "mbed.h"


class NRF52_DigitalOut{
    public:
        NRF52_DigitalOut(uint8_t pin) : pin(pin){ NRF_P0->DIRSET = (uint32_t)1 << pin; }
        ~NRF52_DigitalOut() {}

        inline void operator=(bool state){
            if(state){
                NRF_P0->OUTSET = (uint32_t)1 << pin;
            }
            else{
                NRF_P0->OUTCLR = (uint32_t)1 << pin;
            }
        }
        inline bool operator!() { return !((NRF_P0->OUT >> pin) & 1); }
        inline void toggle() { NRF_P0->OUT ^= (uint32_t)1 << pin; }
        inline operator bool() { return (NRF_P0->OUT >> pin) & 1; }
    private:
        uint8_t pin;
};


class NRF52_DigitalIn{
    public:
        NRF52_DigitalIn(uint8_t pin) : pin(pin){ NRF_P0->DIRCLR = (uint32_t)1 << pin; }
        ~NRF52_DigitalIn() {}

        inline operator bool() { return (NRF_P0->IN >> pin) & 1; }
    private:
        uint8_t pin;
};



#endif // NRF52_DIGITAL_H