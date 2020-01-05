/* 
 * aconno.de
 * Made by Karlo Milicevic and Jurica Resetar @ aconno, 2017
 * All rights reserved 
 *
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include "mbed.h"
 
#define REF_VOLTAGE_V   (3.3)
#define SPI_WRITE       (0x7F)
#define SPI_READ        (0x80)
#define SPI_DUMMY_BYTE  (0x00)

#define TURN_PIN_ON         (1)
#define TURN_PIN_OFF        (0)
#define TURN_LED_ON         (0)
#define TURN_LED_OFF        (1)

static bool randomActive = false;
inline bool startRandom(){ bool oldValue = randomActive; NRF_RNG->TASKS_START = 1; randomActive = true; return oldValue; }
inline bool stopRandom(){ bool oldValue = randomActive; NRF_RNG->TASKS_STOP = 1; randomActive = false; return oldValue; }
int32_t randomNumberGenerator(void *userData, unsigned char *output, size_t size);
void randomNumberGenerator(unsigned char *output, size_t size);
int32_t pcg32_randomNumberGenerator(void *userData, unsigned char *output, size_t size);

inline float clamp(float lower, float upper, float value){
    if(value < lower) return lower;
    if(value > upper) return upper;
    else return value;
}
    
inline uint8_t clamp(uint8_t lower, uint8_t upper, uint8_t value){
    if(value < lower) return lower;
    if(value > upper) return upper;
    else return value;
}

inline void setInput(uint8_t pin){
    NRF_P0->DIRSET = (uint32_t)1 << pin;
}

inline void setOutput(uint8_t pin){
    NRF_P0->DIRCLR = (uint32_t)1 << pin;
}

inline int8_t random8(){ 
    while(NRF_RNG->EVENTS_VALRDY == 0);
    NRF_RNG->EVENTS_VALRDY = 0;
    return NRF_RNG->VALUE;
}

inline int32_t random32(){
    while(NRF_RNG->EVENTS_VALRDY == 0);
    NRF_RNG->EVENTS_VALRDY = 0;
    int result = (int32_t)(NRF_RNG->VALUE << 0);
    while(NRF_RNG->EVENTS_VALRDY == 0);
    NRF_RNG->EVENTS_VALRDY = 0;
    result |= (int32_t)(NRF_RNG->VALUE << 8);
    while(NRF_RNG->EVENTS_VALRDY == 0);
    NRF_RNG->EVENTS_VALRDY = 0;
    result |= (int32_t)(NRF_RNG->VALUE << 16);
    while(NRF_RNG->EVENTS_VALRDY == 0);
    NRF_RNG->EVENTS_VALRDY = 0;
    result |= (int32_t)(NRF_RNG->VALUE << 24);
    return result;
}

struct pcg32_random_state{
    uint64_t state;
    uint64_t inc;
};

union vector3_f;

/** vector3 of int16_t
 */
union vector3_s{
    struct{
        int16_t x, y, z;
    };
    int16_t e[3];
    inline vector3_s() {}
    inline vector3_s(vector3_f v);
};

/** vector3 of float
 */
union vector3_f{
    struct{
        float x, y, z;
    };
    float e[3];
    inline vector3_f() {}
    inline vector3_f(vector3_s v){
        x = (float)v.x;
        y = (float)v.y;
        z = (float)v.z;
    }
};

vector3_s::vector3_s(vector3_f v)
{
    x = (uint16_t)v.x;
    y = (uint16_t)v.y;
    z = (uint16_t)v.z;
}

/*inline void floatToBuffer(float value, uint8_t *buffer){
    *(float *)buffer = value;
}*/
union float2bytes{
    float f; 
    char b[sizeof(float)]; 
};
inline void floatToBuffer(float value, uint8_t *buffer){
    float2bytes temp;
    temp.f = value;
    buffer[0] = temp.b[0];
    buffer[1] = temp.b[1];
    buffer[2] = temp.b[2];
    buffer[3] = temp.b[3];
}


void vector3ToBuffer(vector3_s vector, uint8_t buffer[6]);
void vector3ToBuffer(vector3_f vector, uint8_t buffer[12]);

/** Converts 2 bytes to a single int16_t where MSB is always a full byte while
* size of LSB can change. 2's complement representation is implied. 
*
* example for L=4:
*     MSB:0x55 LSB:0x30 -> 0x0553
*     MSB:0x84 LSB:0x30 -> 0xF843
*
* example for L=2
*     MSB:0x55 LSB:0xC0 -> 0x0157
*/
template<uint8_t L>
inline int16_t convert(int8_t msb, int8_t lsb) {
    int16_t result = ((int16_t)msb << L) | ((uint8_t)lsb >> (8-L));
    if(msb & 0x80) {
        result |= ((uint16_t)0xFF00 << L);
    }
    return result;
}

// vector3_s operations

inline vector3_s operator+(vector3_s l, vector3_s r){
    vector3_s result;
    result.x = l.x + r.x;
    result.y = l.y + r.y;
    result.z = l.z + r.z;
    return result;
}

inline vector3_s operator+=(vector3_s &l, vector3_s r){
    l = l + r;
    return l;
}

inline vector3_s operator-(vector3_s v){
    vector3_s result;
    result.x = -v.x;
    result.y = -v.y;
    result.z = -v.z;
    return result;
}

inline vector3_s operator-(vector3_s l, vector3_s r){
    return l + (-r);
}

inline vector3_s operator-=(vector3_s &l, vector3_s r){
    l = l - r;
    return l;
}

inline vector3_s operator*(vector3_s l, int16_t r){
    vector3_s result;
    result.x = l.x * r;
    result.y = l.y * r;
    result.z = l.z * r;
    return result;
}

inline vector3_s operator*=(vector3_s &l, int16_t r){
    l = l * r;
    return l;
}

inline vector3_s operator/(vector3_s l, int16_t r){
    vector3_s result;
    result.x = l.x / r;
    result.y = l.y / r;
    result.z = l.z / r;
    return result;
}

inline vector3_s operator/=(vector3_s &l, int16_t r){
    l = l / r;
    return l;
}

// vector3_f operations
inline vector3_f operator+(vector3_f l, vector3_f r){
    vector3_f result;
    result.x = l.x + r.x;
    result.y = l.y + r.y;
    result.z = l.z + r.z;
    return result;
}

inline vector3_f operator+=(vector3_f &l, vector3_f r){
    l = l + r;
    return l;
}

inline vector3_f operator-(vector3_f v){
    vector3_f result;
    result.x = -v.x;
    result.y = -v.y;
    result.z = -v.z;
    return result;
}

inline vector3_f operator-(vector3_f l, vector3_f r){
    return l + (-r);
}

inline vector3_f operator-=(vector3_f &l, vector3_f r){
    l = l - r;
    return l;
}

inline vector3_f operator*(vector3_f l, float r){
    l.x *= r;
    l.y *= r;
    l.z *= r;
    return l;
}

inline vector3_f operator*=(vector3_f &l, float r){
    l = l * r;
    return l;
}

inline vector3_f operator/(vector3_f l, float r){
    l.x /= r;
    l.y /= r;
    l.z /= r;
    return l;
}

inline vector3_f operator/=(vector3_f &l, float r){
    l = l / r;
    return l;
}

#endif // UTILITIES_H